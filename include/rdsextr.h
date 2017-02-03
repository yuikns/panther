// Copyright 2014 Yu Jing<yujing5b5d@gmail.com>
#ifndef INCLUDE_RDSEXTR_H_
#define INCLUDE_RDSEXTR_H_

#include <cstdio>
#include <cmath>

// #include <thread>  // std::thread
#include <mutex>  // std::mutex
#include <string>



//#include "leveldb/db.h"
//#include "leveldb/cache.h"
//#include "leveldb/options.h"

#include "argcv/util/timer.h"

#include "rdsextr/gnode.h"

namespace rdsextr {


class Rdsextr {
public :
    Rdsextr(const std::string & m_data, size_t m_t, size_t m_d, double m_epsilon,size_t par_size = 8) : 
        data(m_data),
        T(m_t),
        D(m_d),
        epsilon(m_epsilon),
        par_size(par_size),
        R((size_t)(( (double)0.5/(epsilon * epsilon) )* ((log2( T*(T+1)/2) + 1) + log((double)1/0.1) ) )),
        dis_epsilon(round(epsilon*1000000)),
        network_filename("data/" + data + ".graph"),
        map_filename("data/" + data + ".dict"),
        similar_path_filename("result/"+data+"_"+std::to_string(T)+"_"+std::to_string(D)+"_"+std::to_string(dis_epsilon)+".pathsim"),
        similar_structure_filename("result/"+data+"_"+std::to_string(T)+"_"+std::to_string(D)+"_"+std::to_string(dis_epsilon)+".pathvec"),
        similar_path_file(fopen(similar_path_filename.c_str(),"w")),
        similar_structure_file(fopen(similar_structure_filename.c_str(),"w")){
    }
    
    ~Rdsextr () {
        printf("Rdsextr Finished ...all time cost : %.3f ms\n",timer.all());
        if(_G != NULL)
            delete _G;
        if(similar_path_file != NULL){
            fclose(similar_path_file);
        }
        if(similar_structure_file != NULL){
            fclose(similar_structure_file);
        }
    }

    void start();

    void read_ds_size(char * line , size_t &n, size_t &m); // readMandN
    void load_link();
    
    void gen_random_path_parr();

    void cal_path_sim_parr();

    void save();

    //inline int calR() {
    //    return (int)(((double)0.5/(epsilon * epsilon))*((log2(T*(T-1)/2) + 1) + log((double)1/0.1)));
    //}
    
    // control group
    // 
    void start_ctl(double p, double q);
    void load_link_ctl(double p, double q);
    void gen_random_path_parr_ctl();
    void cal_path_sim_parr_ctl();

private :
    const std::string data;
    size_t T;
    size_t D;
    double epsilon;
    size_t par_size;
    size_t R;
    size_t dis_epsilon; // displayEpsilon
    const std::string network_filename; // network_file
    const std::string map_filename; // map_file
    const std::string similar_path_filename; // file name , pathsim
    const std::string similar_structure_filename; // file name , pathvec
    FILE * similar_path_file;
    FILE * similar_structure_file;
    RGrapgh * _G;
    std::mutex mtx;
    argcv::util::Timer timer;
    size_t N; // Node Size
    size_t M; // Edge Size
    //leveldb::DB* db;
    //leveldb::Options options;
    //std::mutex * _gg_mtx;  // mutex for critical section
};

}  // namespace rdsextr

#endif  // INCLUDE_RDSEXTR_H_
