// Copyright 2015 Yu Jing<yujing5b5d@gmail.com>
#ifndef INCLUDE_RDSEXTR_PARR_EVENTS_H_
#define INCLUDE_RDSEXTR_PARR_EVENTS_H_

#include "rdsextr/gnode.h"

namespace rdsextr {

typedef bool (*rds_parr_event)(RGrapgh * ,size_t,size_t);

bool accum_weight_setter_event(RGrapgh * _G , size_t lower_bound, size_t upper_bound );
bool random_path_generator_event(RGrapgh * _G , size_t lower_bound, size_t upper_bound );
bool pathid_collector_event(RGrapgh * _G , size_t lower_bound, size_t upper_bound );
bool clean_node_in_path_event(RGrapgh * _G , size_t lower_bound, size_t upper_bound);
bool path_sim_calculator_event(RGrapgh * _G , size_t lower_bound, size_t upper_bound );

//bool rds_parr_event_test(RGrapgh * _G , size_t lower_bound, size_t upper_bound);

bool rds_parr_event_manager(RGrapgh * _G , size_t lower_bound, size_t upper_bound,rds_parr_event event,size_t thread_size);

bool complete_path_generator_event_ctl(RGrapgh * _G , size_t lower_bound, size_t upper_bound );
bool random_path_generator_event_ctl(RGrapgh * _G , size_t lower_bound, size_t upper_bound );

inline size_t rpg_sim_bs(const std::vector<NeighborNode> & nabors, double val){
    //accum_weight
    if(val < nabors[0].accum_weight) return 0;
    size_t low = 1; 
    size_t high = nabors.size() - 1; // nabors[nabors.size() - 1] shall always 1
    size_t middle;
    while(low <= high){
        middle = low + (high-low)/2;
        if(val <= nabors[middle].accum_weight && val > nabors[middle-1].accum_weight){
            return middle;
        }
        if(val < nabors[middle-1].accum_weight){
            high = middle - 1;
        }
        if(val > nabors[middle].accum_weight){
            low = middle + 1;
        }
    }
    return 0; // error
}


inline std::string double2string(double d) {
    char s[50];
    snprintf(s,49,"%.15f",d);
    return std::string(s);
}


}

#endif  // INCLUDE_RDSEXTR_PARR_EVENTS_H_
