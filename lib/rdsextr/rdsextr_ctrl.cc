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

#define VERBOSE_LINK 0

namespace rdsextr {
    
    void Rdsextr::load_link_ctl(double p, double q) {
        FILE *fp = fopen(network_filename.c_str(), "r");
        char line[100];
        fgets(line,1000, fp);
        read_ds_size(line,N,M);
        printf("N: %zu, M: %zu, R=%zu\n",N, M, R);
        fflush(NULL);
        _G = new RGrapgh(N,0);
        printf("graph space allocated\n");
        fflush(NULL);

        // Global Static Environment Setting
        _G->T = T;
        _G->D = D;
        
        _G->CTL_P = p;
        _G->CTL_Q = q;
        
        int64_t th = RAND_MAX * p;
        while(!feof(fp)){
            int a, b;
            double c;
            fgets(line,100, fp);
            sscanf(line, "%d\t%d\t%lf", &a, &b,&c);
            if(rand() <= th) {
                _G->get_node(a)->neighbors.push_back(NeighborNode(b,c,c));
                _G->get_node(b)->neighbors.push_back(NeighborNode(a,c,c));
#if VERBOSE_LINK
                printf("link: %d => %d (%lf) [Yes]\n", a, b, c);
#endif
            } else {
                // ignore
#if VERBOSE_LINK
                printf("link: %d => %d (%lf) [No ]\n", a, b, c);
#endif
            }
        }
    }

    void Rdsextr::start_ctl(double p, double q) {
        //eng.seed((unsigned long long)time(NULL));
        printf("initing ... \n");
        fflush(NULL);
        printf("Rdsextr Starting ...\ndata:%s, T:%zu, D:%zu, R:%zu, E:%f, p: %f, q: %f\n", 
            data.c_str(), T, D, R, epsilon,
            p, q);
        fflush(NULL);
        printf("displayed epsilon %zu , network file [%s] , map file [%s]\n",
                        dis_epsilon, network_filename.c_str(), map_filename.c_str());
        fflush(NULL);
        printf("result file stored in :\n::path: [%s]\n::structure: [%s]\n",
                        similar_path_filename.c_str(), similar_structure_filename.c_str());
        fflush(NULL);
        timer.label("load_link_start");
        load_link_ctl(p, q);
        timer.label("load_link_end");
        printf("Time of loading edges = %lf ms\n",timer.between("load_link_start","load_link_end"));
        fflush(NULL); // flush cache
        timer.label("gen_random_path_start");
        //gen_random_path();
        gen_random_path_parr_ctl();
        timer.label("gen_random_path_end");
        printf("Time of generating random paths = %lf ms\n",timer.between("gen_random_path_start","gen_random_path_end"));
        fflush(NULL); // flush cache
        timer.label("cal_path_sim_start");
        cal_path_sim_parr();
        timer.label("cal_path_sim_end");
        printf("Time of calcualating pathsim = %lf ms\n",timer.between("cal_path_sim_start","cal_path_sim_end"));
        fflush(NULL); // flush cache
    }
    
    void Rdsextr::gen_random_path_parr_ctl(){
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
        if(_G -> CTL_Q >= 1.0) {
            status = rds_parr_event_manager(_G,0,N,complete_path_generator_event_ctl,par_size);
        } else {
            status = rds_parr_event_manager(_G,0,N,random_path_generator_event_ctl,par_size);
        }
        printf("[rpath gene] stopped, time cost : %.3f ms [%s] \n",timer.from("gen_random_path::rpg_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache

        R =  _G -> reset_psize();
        printf("[rpath gene] length of path: %lu\n", R);
        
        timer.label("gen_random_path::pic_start");
        printf("[pathids] starting...\n");
        fflush(NULL); // flush cache
        status = rds_parr_event_manager(_G,0,N,pathid_collector_event,par_size);
        printf("[pathids] stopped, time cost : %.3f ms [%s] \n",timer.from("gen_random_path::pic_start"),(status?"OK":"FAILED"));
        fflush(NULL); // flush cache
        
    }

}  // namespace rdsextr
