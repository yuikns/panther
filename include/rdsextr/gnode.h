// Copyright 2015 Yu Jing<yujing5b5d@gmail.com>
#ifndef LIB_RDSEXTR_GNODE_H_
#define LIB_RDSEXTR_GNODE_H_

#include <vector>
#include <set>
#include <queue> // queue
#include <mutex>

//#include "moodycamel/concurrentqueue.h"

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"

namespace rdsextr {

//using moodycamel::ConcurrentQueue;

class NeighborNode {
public:
    size_t id;
    double weight;
    double accum_weight;
    NeighborNode(size_t id,double weight,double accum_weight):
        id(id),
        weight(weight),
        accum_weight(accum_weight) {
    }
    ~NeighborNode(){
    }
};

class GNode{
public:
    std::vector<NeighborNode> neighbors;
    std::vector<size_t> path;
    //ConcurrentQueue<int> path_cq;
    //std::queue<int> path_q;
};

class GPath{
public:
    std::vector<size_t> node_v;
    //ConcurrentQueue<int> node_cq;
    //std::queue<int> node_q;
    //std::set<int> node_s;
    GPath(std::vector<size_t>& nodes):node_v(nodes){}
    GPath(){}
};

class RGrapgh{
public:
    
    // N: Node Size
    // R: Path Size
    RGrapgh(size_t node_size,size_t path_size,const std::string &dbdir = "similar_info.db",
            const std::string & similar_path_filename = "similar_path::",
            const std::string & similar_structure_filename = "similar_structure::"):
        node_size(node_size),
        path_size(path_size),
        nodes(std::vector<GNode>(node_size)),
        paths(std::vector<GPath>(path_size)),
        dbdir(dbdir),
        similar_path_filename(similar_path_filename),
        similar_structure_filename(similar_structure_filename){
            // leveldb init
            options.create_if_missing = true;  // create if not exists
            options.block_cache = leveldb::NewLRUCache(30 * 1048576L);  // 1000MB cache

            DestroyDB(dbdir,leveldb::Options());
            //leveldb::Status status =
            leveldb::DB::Open(options, dbdir, &db);
            
            
    }
    
    ~RGrapgh(){
        //delete [] _nodes;
        //delete [] _paths;
        delete db;
    }

    GNode * get_node(size_t offset){
        if(offset >= node_size) {
            return nullptr;
        }else{
            return &(nodes[offset]);
        }
    }

    GPath * get_path(size_t offset){
        if(offset >= path_size) {
            return nullptr;
        }else{
            return &(paths[offset]);
        }
    }

    size_t nsize() const { return node_size;}
    size_t psize() const { return path_size;}
    
    size_t & R() { return path_size; }

    size_t T;
    size_t D;
    leveldb::DB* db;
    leveldb::Options options;

    const std::string & get_similar_path_filename() const {
        return similar_path_filename;
    }

    const std::string & get_similar_structure_filename() const {
        return similar_structure_filename;
    }
    
    double CTL_P;
    double CTL_Q;
    
    size_t reset_psize() {
        path_size = paths.size();
        return path_size;
    }
    
    void append_path(GPath& path){
        path_mtx.lock();
        paths.push_back(path);
        path_mtx.unlock();
    }
    
    void append_path(std::vector<size_t>& node_v) {
        if(node_v.size() > 1) {
            path_mtx.lock();
            paths.emplace_back(node_v);
            path_mtx.unlock();
        }
    }

private:
    size_t node_size;
    size_t path_size;
    //GNode * _nodes;
    //GPath * _paths;
    std::vector<GNode> nodes;
    std::vector<GPath> paths;
    const std::string dbdir;
    const std::string similar_path_filename; // file name , pathsim
    const std::string similar_structure_filename; // file name , pathvec
    std::mutex path_mtx;
};

}  // namespace rdsextr


#endif  // LIB_RDSEXTR_GNODE_H_

