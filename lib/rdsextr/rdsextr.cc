// Copyright 2015 Yu Jing<yujing5b5d@gmail.com>
#include "rdsextr.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <vector>
#include <set>
#include <map>
#include <utility> // pair
#include <mutex> // std::lock_guard , mutex
#include <random>


//#include "rdsextr/pc.h"
#include "rdsextr/gnode.h"
#include "rdsextr/parr_events.h"
#include "rdsextr/util.h"

using std::vector;
using std::set;
using std::map;
using std::pair;

namespace rdsextr {

    void Rdsextr::start() {
        //eng.seed((unsigned long long)time(NULL));
        printf("initing ... \n");
        fflush(NULL);
        printf("Rdsextr Starting ...\ndata:%s, T:%zu, D:%zu, R:%zu, E:%f\n", data.c_str(), T, D, R, epsilon);
        fflush(NULL);
        printf("displayed epsilon %zu , network file [%s] , map file [%s]\n",
                        dis_epsilon, network_filename.c_str(), map_filename.c_str());
        fflush(NULL);
        printf("result file stored in :\n::path: [%s]\n::structure: [%s]\n",
                        similar_path_filename.c_str(), similar_structure_filename.c_str());
        fflush(NULL);
        timer.label("load_link_start");
        load_link();
        timer.label("load_link_end");
        printf("Time of loading edges = %lf ms\n",timer.between("load_link_start","load_link_end"));
        fflush(NULL); // flush cache
        timer.label("gen_random_path_start");
        //gen_random_path();
        gen_random_path_parr();
        timer.label("gen_random_path_end");
        printf("Time of generating random paths = %lf ms\n",timer.between("gen_random_path_start","gen_random_path_end"));
        fflush(NULL); // flush cache
        timer.label("cal_path_sim_start");
        cal_path_sim_parr();
        timer.label("cal_path_sim_end");
        printf("Time of calcualating pathsim = %lf ms\n",timer.between("cal_path_sim_start","cal_path_sim_end"));
        fflush(NULL); // flush cache
    }

    void Rdsextr::read_ds_size(char * line , size_t &n, size_t &m) {
        char * k = strtok(line,"\t");
        if( k == NULL) {
           fprintf(stderr,"[read_ds_size] error unknown val : %s\n",line);
           return;
        }
        n = atoll(k); 
        k =  strtok(NULL,"\t");
        if( k == NULL) {
           fprintf(stderr,"[read_ds_size] erroe unknown val : %s\n",line);
            return ;
        }
        m = atoll(k);
    }

    void Rdsextr::load_link() {
        FILE *fp = fopen(network_filename.c_str(), "r");
        char line[100];
        fgets(line,1000, fp);
        read_ds_size(line,N,M);
        printf("N: %zu, M: %zu, R=%zu\n",N, M, R);
        fflush(NULL);
        _G = new RGrapgh(N,R);
        printf("graph space allocated\n");
	fflush(NULL);

        // Global Static Environment Setting
        _G->T = T;
        _G->D = D;
        //_G->R = R;

        int num_edge = 0;
        while(!feof(fp)){
            if(num_edge++ < M) {
                int a, b;
                double c;
                fgets(line,100, fp);
                if(sscanf(line, "%d\t%d\t%lf", &a, &b,&c) != -1) {
                    _G->get_node(a)->neighbors.push_back(NeighborNode(b,c,c));
                    _G->get_node(b)->neighbors.push_back(NeighborNode(a,c,c));
                }
            }
        }
    }

    void Rdsextr::gen_random_path_parr(){
        printf("sample size = %zu\n", R);
        printf("thread size = %zu\n", par_size);
        timer.label("gen_random_path::aws_start");
        printf("[weight] starting...\n");
        fflush(NULL); // flush cache
        bool status = rds_parr_event_manager(_G,0,N,accum_weight_setter_event,par_size);
        printf("[weight] stopped, time cost : %.3f ms [%s] \n",timer.from("gen_random_path::aws_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
        
        timer.label("gen_random_path::rpg_start");
        printf("[rpath gene] starting...\n");
        fflush(NULL); // flush cache
        status = rds_parr_event_manager(_G,0,R,random_path_generator_event,par_size);
        printf("[rpath gene] stopped, time cost : %.3f ms [%s] \n",timer.from("gen_random_path::rpg_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
        
        timer.label("gen_random_path::pic_start");
        printf("[pathids] starting...\n");
        fflush(NULL); // flush cache
        status = rds_parr_event_manager(_G,0,N,pathid_collector_event,par_size);
        printf("[pathids] stopped, time cost : %.3f ms [%s] \n",timer.from("gen_random_path::pic_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
        
    }
    
    void Rdsextr::cal_path_sim_parr(){
        // remove duplicate data in nodes
        // only need when U use 'vector'
        timer.label("cal_path_sim_parr::RD");
        printf("[clean node in paths] starting...\n");
        fflush(NULL); // flush cache
        //path start -> path end
        bool status = rds_parr_event_manager(_G,0,R,clean_node_in_path_event,par_size);
        printf("[clean node in paths] stopped, time cost : %.3f ms [%s] \n",
            timer.from("cal_path_sim_parr::RD"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
        
        timer.label("cal_path_sim_parr::psc_start");
        printf("[sim cal] starting...\n");
        fflush(NULL); // flush cache
        
        status = rds_parr_event_manager(_G,0,N,path_sim_calculator_event,par_size);
        printf("[sim cal] stopped, time cost : %.3f ms [%s] \n",timer.from("cal_path_sim_parr::psc_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
    }
    
    

    void Rdsextr::save(){
        timer.label("save_all_start");
        printf("[save result file] starting...\n");
        fflush(NULL); // flush cache
        for(size_t i = 0 ; i < N ; i++){
            std::string similar_path_list_key(_G->get_similar_path_filename());
            similar_path_list_key += std::to_string(i);
            std::string similar_structure_list_key(_G->get_similar_structure_filename());
            similar_structure_list_key += std::to_string(i);
            
            std::string line;
            bool status = ldb_get(_G->db,similar_path_list_key,&line);
            fprintf(similar_path_file,"%s\n",status?line.c_str():"");
            status = ldb_get(_G->db,similar_structure_list_key,&line);
            fprintf(similar_structure_file,"%s\n",status?line.c_str():"");
        }
        timer.label("save_all_end");
        printf("[save result file] time cost %f ms\n",timer.between("save_all_start","save_all_end"));
        fflush(NULL); // flush cache
    }


















}  // namespace rdsextr
