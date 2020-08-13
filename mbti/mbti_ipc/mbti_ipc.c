#include "mbti_ipc.h"

double ipc_test(int topology, int processes, int iter, int cores){

    double measure_time = 0;

    switch(topology){
    	case 1: // Ping-pong
            return mq_pingpong(processes, iter, cores);
        default:
            return 0;
    }

    return measure_time;
}
