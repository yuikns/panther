#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/stat.h>

#include <utility>


#include "argcv/util/timer.h"
#include "rdsextr.h"
//#include "rdsextr/util.h"
//#include "rdsextr/gnode.h"
//#include "rdsextr/parr_events.h"

//#include "ThreadPool.h"

//#include <vector>
//#include <chrono>

//using rdsextr::MinHeap;
using namespace rdsextr;


int rds_start(int argc, char* argv[]) {
    srand((unsigned)time(NULL));
    int par_size = 4;
    if(argc <  5 ) {;
        printf("Usage:\n\t%s data T D epsilon [thread_number]\n",argv[0]);
        return -1;
    } else {
        char * data = argv[1];
        size_t T = atoi(argv[2]);
        size_t D = atoi(argv[3]);
        double epsilon = atof(argv[4]);
        if(T == 0 || D == 0 || epsilon == 0 ) {
            printf("Usage:\n\t%s data T D epsilon [thread_number]\n",argv[0]);
            return -1;
        } else {
            if(argc > 5 ) {
                par_size = atoi(argv[5]);
            }
            mkdir("result",0755);
            rdsextr::Rdsextr r(data, T, D, epsilon, par_size);
            r.save();
        }
    }
    return 0;
}


int main(int argc, char* argv[]){
    srand((unsigned)time(NULL));
    argcv::util::Timer allTimer;
    int result = rds_start(argc,argv);
    printf("all done... all cost : %f ms\n",allTimer.all());
    return result;
}







