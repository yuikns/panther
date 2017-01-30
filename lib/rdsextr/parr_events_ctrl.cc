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
        for(auto & neighbor : _node->neighbors ) {
            size_t nxid = neighbor.id;
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
        for(auto & neighbor : _node->neighbors ) {
            size_t nxid = neighbor.id;
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




}


