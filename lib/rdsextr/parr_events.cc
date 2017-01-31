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

//#define SINGLE_THREAD_DEBUG_MODE 1


namespace rdsextr {

using std::vector;
using std::set;
using std::map;
using std::string;

bool accum_weight_setter_event(RGrapgh * _G , size_t node_start, size_t node_end ) {
    for(size_t item = node_start; item < node_end ; item ++) {
        GNode * _node = _G->get_node(item);
        if(_node != NULL) {
            double sum = 0;
            //std::vector<NeighborNode> * _neighbors = &(_node->neighbors);
            //for(std::vector<NeighborNode>::iterator it=_neighbors->begin(); it != _neighbors->end() ; it++){
            //    sum += it->weight;
            //}
            //std::map<size_t, NeighborNode> & neighbors = _node->neighbors;
            std::map<size_t, double> & weights = _node->weights;
            for(auto & weight : weights) {
                sum += weight.second;
            }
            sum = sum > 0 ? sum : 0.001;
            double priv = 0.0;
            for(auto & weight : weights) {
                sum += weight.second;
                double acw = weight.second / sum + priv;
                _node->neighbors.emplace_back(weight.first, acw );
                priv = acw;
            }
        }
    }
    return true;
}

bool random_path_generator_event(RGrapgh * _G , size_t pathid_start, size_t pathid_end ) {
    size_t nsize = _G->nsize();
    for(size_t pathid = pathid_start; pathid < pathid_end ; pathid++ ){
        size_t curr_node = rand() % nsize;// 0 - N-1
        GPath * _path = _G->get_path(pathid);
        /*
        set<size_t> * _nodes = &(_path->node_s);
        _nodes->insert(curr_node);
        for(size_t t = 0 ; t < _G->T ; t ++ ) {
            GNode * _node = _G -> get_node(curr_node);
            if(_node->neighbors.size() == 0 ) break;
            //double rand_val = uni();
            double rand_val = (double)rand() / RAND_MAX;
            size_t neighbor_index = rpg_sim_bs(_node->neighbors,rand_val);
            curr_node = _node->neighbors[neighbor_index].id;
            _nodes->insert(curr_node);
        }
        */
        vector<size_t> *_node_v = &(_path->node_v);
        _node_v->push_back(curr_node);
        for(size_t t = 0 ; t < _G->T ; t ++ ) {
            GNode * _node = _G -> get_node(curr_node);
            if(_node->neighbors.size() == 0 ) break;
            //double rand_val = uni();
            double rand_val = (double)rand() / RAND_MAX;
            size_t neighbor_index = rpg_sim_bs(_node->neighbors,rand_val);
            curr_node = _node->neighbors[neighbor_index].id;
            _node_v->push_back(curr_node);
        }
    }
    return true;
}



bool pathid_collector_event(RGrapgh * _G , size_t node_start, size_t node_end ) {
    for(size_t pid = 0 ; pid < _G->R() ; pid ++ ){
        GPath * _path = _G->get_path(pid);
        if(_path != nullptr ) {
            /*
            set<size_t> *_node_s = &(_path->node_s);
            for (std::set<size_t>::const_iterator it=_node_s->begin(); it!= _node_s->end(); ++it) {
                if((*it) < node_end && (*it) >= node_start) {
                    _G->get_node(*it)->path.push_back(pid);
                }
            }
            */
            vector<size_t> *_node_v = &(_path->node_v);
            for (std::vector<size_t>::const_iterator it=_node_v->begin(); it!= _node_v->end(); ++it) {
                if((*it) < node_end && (*it) >= node_start) {
                    _G->get_node(*it)->path.push_back(pid);
                }
            }
        }
    }
    return true;
}


bool clean_node_in_path_event(RGrapgh * _G , size_t pid_start, size_t pid_end) {
    for(size_t i = pid_start; i < pid_end; i ++ ) {
        vector<size_t> * _node_v = &(_G->get_path(i)->node_v);
        assert(_node_v != nullptr);
        set<size_t> node_s(_node_v->begin(),_node_v->end());
        _node_v->assign( node_s.begin(), node_s.end() );
    }
    return true;
}

bool path_sim_calculator_event(RGrapgh * _G , size_t nodeid_start, size_t nodeid_end){
    map<string,string> data_to_save;
    leveldb::DB* db = _G->db;
    for(size_t i = nodeid_start; i < nodeid_end ; i++ ){
        map<size_t, size_t> nodesInSamePath;
        GNode * _current_node = _G->get_node(i);
        std::vector<size_t> * _current_path = &(_current_node->path);
        // key : node id , value : path id size
        for(vector<size_t>::iterator iter = _current_path->begin();
            iter != _current_path->end();
            iter++) {
                /*
                // G->path->node is set
                set<size_t> * _ns = &(_G->get_path(*iter)->node_s);
                //nodeset.insert(_G->get_path(*iter)->node.begin(),_G->get_path(*iter)->node.end());
                for(set<size_t>::const_iterator it = _ns->begin();
                    it != _ns->end();
                    it++) {
                        //if(!nodesInSamePath.count(*it))
                        if(nodesInSamePath.find(*it) == nodesInSamePath.end())
                          nodesInSamePath[*it]=0;
                        nodesInSamePath[*it]++;
                }
                */
                // G->path->node is vector
                vector<size_t> * _node_v = &(_G->get_path(*iter)->node_v);
                //nodeset.insert(_G->get_path(*iter)->node.begin(),_G->get_path(*iter)->node.end());
                for(vector<size_t>::const_iterator it = _node_v->begin();
                    it != _node_v->end();
                    it++) {
                        //if(!nodesInSamePath.count(*it))
                        if(nodesInSamePath.find(*it) == nodesInSamePath.end())
                          nodesInSamePath[*it]=0;
                        nodesInSamePath[*it]++;
                    
                }
        }

        MinHeap<std::pair<size_t,size_t>> top_D_nodes(_G->D,rdsextr::pair_compare_by_value<size_t,size_t>);

        for (std::map<size_t,size_t>::iterator it=nodesInSamePath.begin(); it!=nodesInSamePath.end(); ++it)
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

/*
bool rds_parr_event_test(RGrapgh * _G , size_t lower_bound, size_t upper_bound) {
    size_t sum = 0;
    for(size_t i = lower_bound; i < upper_bound ; i ++ ){
        sum += i;
        prsize_tf("[%zu-%zu] offset : %zu\n",lower_bound, upper_bound, i);
    }
    return true;
}
*/


bool rds_parr_event_manager(RGrapgh * _G , size_t lower_bound, size_t upper_bound,rds_parr_event event,size_t thread_size) {
#ifdef SINGLE_THREAD_DEBUG_MODE
    ThreadPool pool(thread_size);
    size_t slice_len = (upper_bound - lower_bound) / thread_size + 1;
    bool all_done = true;
    std::vector< std::future<bool> > results;
    for(size_t i = 0 ; i < thread_size ; i++ ) {
        size_t c_lower_bound = i * slice_len;
        size_t c_upper_bound = (i+1) * slice_len;
        if(c_upper_bound > upper_bound ) {
            c_upper_bound = upper_bound;
        }
        bool c_status = event(_G,c_lower_bound,c_upper_bound);
        if(!c_status) {
            all_done = false;
        }
    }
    return all_done;
#else
    ThreadPool pool(thread_size);
    size_t slice_len = (upper_bound - lower_bound) / thread_size + 1;
    bool all_done = true;
    std::vector< std::future<bool> > results;
    for(size_t i = 0 ; i < thread_size ; i++ ) {
        results.emplace_back(
            pool.enqueue([i,lower_bound,upper_bound,slice_len,_G,event] {
                size_t c_lower_bound = i * slice_len;
                size_t c_upper_bound = (i+1) * slice_len;
                if(c_upper_bound > upper_bound ) {
                    c_upper_bound = upper_bound;
                }
                return event(_G,c_lower_bound,c_upper_bound);
            })
        );
    }
    
    for(auto && result: results){
        bool c_status = static_cast<bool>(result.get());
        if(!c_status){
            all_done = false;
        }
    }
    return all_done;
#endif // SINGLE_THREAD_DEBUG_MODE
}

}


