#include "mbti_ipc.h"

double ipc_test(int topology, int processes, int iter, int cores){

    double measure_time = 0;

    switch(topology){
    	case 1: // Ping-pong
            return mq_pingpong(processes, iter, cores);
        default:
            return 0;
    }

    if(!measure_time) {
        PRINTERROR("IPC TEST EXECUTION FAIL\n");
    }
    else {
        PRINTRESULT("topology:%d, process_pairs:%d, iteration:%d, cores:%d, measure_time_per_pair:%lf\n", topology, processes, iter, cores, measure_time);
    }
    return measure_time;
}
