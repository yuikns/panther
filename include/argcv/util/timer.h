// Copyright 2014 Yu Jing<yujing5b5d@gmail.com>
#ifndef INCLUDE_ARGCV_UTIL_TIMER_H_
#define INCLUDE_ARGCV_UTIL_TIMER_H_

#include <chrono>
#include <map>
#include <string>
//#include <vector>

//using std::vector;

//using std::map;

namespace argcv {
namespace util {

typedef std::chrono::high_resolution_clock hrclock;
typedef std::chrono::duration<double, std::milli> mil;
typedef std::chrono::duration<int64_t, std::ratio<1,1000000000>> nal;
typedef std::chrono::time_point<hrclock, nal> time_point;

class Timer {
public:
    Timer() {
        st_point = hrclock::now();
    }

    ~Timer() {
    }

    void label(const std::string & k) {
        tmap.insert(std::make_pair(k,hrclock::now()));
    }

    // start to now
    double all() {
        return mil(hrclock::now() - st_point).count();
    }

    double from(const std::string & k) {
        std::map<std::string,time_point>::const_iterator p = tmap.find(k);
        return p == tmap.end() ? -1 : mil(hrclock::now() - p->second).count() ;
    }

    double to(const std::string & k) {
        std::map<std::string,time_point>::const_iterator p = tmap.find(k);
        return p == tmap.end() ? -1 : mil(p->second - st_point).count() ;
    }

    double between(const std::string & t , const std::string & f) {
        std::map<std::string,time_point>::const_iterator p0 = tmap.find(t);
        std::map<std::string,time_point>::const_iterator p1 = tmap.find(f);
        return p0 == tmap.end() || p1 == tmap.end() ?
            -1 :
            (p0->second > p1->second ?
                mil(p0->second - p1->second).count() :
                mil(p1->second - p0->second).count());
    }

protected:
    //std::vector<time_point> tms;
    time_point st_point; // start point
    std::map<std::string,time_point> tmap;
};


}  // namespace util
}  // namespace argcv

#endif  // INCLUDE_ARGCV_UTIL_TIMER_H_
