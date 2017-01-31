// Copyright 2015 Yu Jing<yujing5b5d@gmail.com>
#include "rdsextr/parr_events.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>            // std::time


 
#include <pthread.h> // thread
#include <unistd.h> // usleep
 
#include <string>
 
#include <mutex> // std::lock_guard , mutex
#include <queue> // queue
#include <vector>
#include <set>
#include <map>
#include <thread>

//#include <boost/random/linear_congruential.hpp>
//#include <boost/random/uniform_real.hpp>
//#include <boost/random/variate_generator.hpp>

#include "argcv/util/timer.h"
#include "rdsextr/gnode.h"
#include "rdsextr/util.h"

#include "ThreadPool.h"

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"


#define VERBOSE_PATH 1

namespace rdsextr {

using std::vector;
using std::set;
using std::map;
using std::string;

// exact results
// for control group

std::mutex path_debug_mtx;

// currently no escape
void complete_discov(size_t curr_node, set<size_t> escape_nodes, vector<size_t> path_nodes, int len, RGrapgh * _G) {
    path_nodes.push_back(curr_node);
    //escape_nodes.insert(curr_node);
    if(len > 1) {
        GNode * _node = _G -> get_node(curr_node);
        int cnt_discov = 0;
        for(auto & id_weight : _node->weights) {
            size_t nxid = id_weight.first;
            //if(escape_nodes.find(nxid) == escape_nodes.end()) {
            complete_discov(nxid, escape_nodes, path_nodes, len - 1, _G);
            cnt_discov++;
            //}
        }
        if(cnt_discov == 0) {
            // terminate discovery before T comes
#if VERBOSE_PATH
            if(path_nodes.size() > 1) {
                path_debug_mtx.lock();
                printf("[C:PATH:M] ");
                for(auto id: path_nodes) {
                    printf("%lu -> ", id);
                }
                printf("\n");
                path_debug_mtx.unlock();
            }
#endif
            _G->append_path(path_nodes);
        }
    } else {
#if VERBOSE_PATH
        if(path_nodes.size() > 1) {
            path_debug_mtx.lock();
            printf("[C:PATH:E] ");
            for(auto id: path_nodes) {
                printf("%lu -> ", id);
            }
            printf("\n");
            path_debug_mtx.unlock();
        }
#endif
        _G->append_path(path_nodes);
    }
}

bool complete_path_generator_event_ctl(RGrapgh * _G , size_t nodeid_start, size_t nodeid_end) {
    set<size_t> escape_nodes;
    vector<size_t> path_nodes;
    for(size_t i = nodeid_start; i < nodeid_end ; i++ ){
        complete_discov(i, escape_nodes, path_nodes, _G->T, _G);
    }
    return true;
}



// currently no escape
void random_discov(size_t curr_node, set<size_t> escape_nodes, vector<size_t> path_nodes, int len, RGrapgh * _G, int64_t threshold) {
    path_nodes.push_back(curr_node);
    //escape_nodes.insert(curr_node);
    if(len > 1) {
        GNode * _node = _G -> get_node(curr_node);
        int cnt_discov = 0;
        for(auto & id_weight : _node->weights) {
            size_t nxid = id_weight.first;
            //if(escape_nodes.find(nxid) == escape_nodes.end()) {
            if(rand() <= threshold) {
                random_discov(nxid, escape_nodes, path_nodes, len - 1, _G, threshold);
                cnt_discov++;
            }
            //}
        }
        if(cnt_discov == 0) {
            // terminate discovery before T comes
#if VERBOSE_PATH
            if(path_nodes.size() > 1) {
                path_debug_mtx.lock();
                printf("[R:PATH:M] ");
                for(auto id: path_nodes) {
                    printf("%lu -> ", id);
                }
                printf("\n");
                path_debug_mtx.unlock();
            }
#endif
            _G->append_path(path_nodes);
        }
    } else {
#if VERBOSE_PATH
        if(path_nodes.size() > 1) {
            path_debug_mtx.lock();
            printf("[R:PATH:E] ");
            for(auto id: path_nodes) {
                printf("%lu -> ", id);
            }
            printf("\n");
            path_debug_mtx.unlock();
        }
#endif
        _G->append_path(path_nodes);
    }
}


bool random_path_generator_event_ctl(RGrapgh * _G , size_t nodeid_start, size_t nodeid_end) {
    set<size_t> escape_nodes;
    vector<size_t> path_nodes;
    int64_t threshold = RAND_MAX * _G->CTL_Q;
    for(size_t i = nodeid_start; i < nodeid_end ; i++ ){
        random_discov(i, escape_nodes, path_nodes, _G->T, _G, threshold);
    }
    return true;
}



bool path_sim_calculator_event_ctl(RGrapgh * _G , size_t nodeid_start, size_t nodeid_end){
    map<string,string> data_to_save;
    leveldb::DB* db = _G->db;
    for(size_t i = nodeid_start; i < nodeid_end ; i++ ){
        map<size_t, size_t> nodes_with_weight_in_same_path;
        GNode * _current_node = _G->get_node(i);
        std::vector<size_t> * _current_path = &(_current_node->path);
        // key : node id , value : path id size
        for(vector<size_t>::iterator iter = _current_path->begin();
            iter != _current_path->end();
            iter++) {
                vector<size_t> * _node_v = &(_G->get_path(*iter)->node_v);
                //nodeset.insert(_G->get_path(*iter)->node.begin(),_G->get_path(*iter)->node.end());
                for(vector<size_t>::const_iterator it = _node_v->begin();
                    it != _node_v->end();
                    it++) {
                        if(nodes_with_weight_in_same_path.find(*it) == nodes_with_weight_in_same_path.end())
                          nodes_with_weight_in_same_path[*it]=0;
                        //printf("%lu => %lu, weight: %f \n", i, *it, _current_node->weight_to(*it));
                        nodes_with_weight_in_same_path[*it] += _current_node->weight_to(*it);
                }
        }

        MinHeap<std::pair<size_t,size_t>> top_D_nodes(_G->D,rdsextr::pair_compare_by_value);

        for (std::map<size_t,size_t>::iterator it=nodes_with_weight_in_same_path.begin(); it!=nodes_with_weight_in_same_path.end(); ++it)
            top_D_nodes.push(*it);

        std::pair<size_t,size_t> val;
        vector<std::pair<size_t,size_t>> top_node_vect;
        while(top_D_nodes.pop(val)) {
            top_node_vect.push_back(val);
        }
        vector_reverse(top_node_vect);
        std::string similar_path_list_key(_G->get_similar_path_filename());
        similar_path_list_key += std::to_string(i);
        std::string similar_structure_list_key(_G->get_similar_structure_filename());
        similar_structure_list_key += std::to_string(i);

        std::string similar_path_list_val("");
        std::string similar_structure_list_val("");
        if(top_node_vect.size() > 0 ) {
            //similar_path_list_val += std::to_string(top_node_vect[0].first) + ":"+ std::to_string((double)top_node_vect[0].second/_G->R);
            similar_path_list_val += std::to_string(top_node_vect[0].first) + ":"+ double2string((double)top_node_vect[0].second/_G->R());
            similar_structure_list_val += std::to_string(top_node_vect[0].second);
            for(size_t ix = 1 ; ix < top_node_vect.size(); ix++ ) {
                similar_path_list_val += " ";
                //similar_path_list_val += std::to_string(top_node_vect[ix].first) + ":"+ std::to_string((double)top_node_vect[ix].second/_G->R);
                similar_path_list_val += std::to_string(top_node_vect[ix].first) + ":"+ double2string((double)top_node_vect[ix].second/_G->R());
    
                similar_structure_list_val += " ";
                similar_structure_list_val += std::to_string(top_node_vect[ix].second);
            }
            for(size_t ix = top_node_vect.size() ; ix < _G->D; ix++ ) {
                similar_structure_list_val += " 0";
            }
        } else {
            similar_structure_list_val += "0";
            for(size_t ix = 1 ; ix < _G->D; ix++ ) {
                similar_structure_list_val += " 0";
            }
        }
        // printf("~~~~~~~~%s\n", similar_path_list_val.c_str());
        //ldb_set(db,similar_path_list_key,similar_path_list_val);
        //ldb_set(db,similar_structure_list_key,similar_structure_list_val);
        data_to_save.insert(std::make_pair(similar_path_list_key,similar_path_list_val));
        data_to_save.insert(std::make_pair(similar_structure_list_key,similar_structure_list_val));

        // printf("======%s\n", data_to_save[similar_path_list_key].c_str());
        
        if(i % 100000 == 0 && data_to_save.size() > 0) {
            ldb_batch_add(db,data_to_save);
            data_to_save.clear();
        }
    }
    if(data_to_save.size() > 0) {
        ldb_batch_add(db,data_to_save);
    }
    return true;
}


}


