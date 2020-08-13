#include "mbti.h"

int main(int argc, char *argv[]){

    int mode, topology, pairs, iter, cores;

    /* 
    * 1. Mode
    * 2. Topology
    * 3. Number of pairs
    * 4. Number of iterations
    * 5. Number of cores
    */

    if(argc != 6){
        printf("[ERROR] argc is must be 5 (Input: %d)\n", argc);
        return -1;
    }

    for(int i = 1; i < argc; i++){
        if(!atoi(argv[i])){
            printf("[Error] argv[%d] must be positive integer (Input: %s)\n", i, argv[i]);
            return -1;
        }
    }
    
    mode = atoi(argv[1]);
    topology = atoi(argv[2]);
    pairs = atoi(argv[3]);
    iter = atoi(argv[4]);
    cores = atoi(argv[5]);

    double measure_time = 0;
    switch(mode){
        case 1: // Signal
            measure_time = sig_test(topology, pairs, iter, cores);
            break;
        case 2: // IPC
	    measure_time = ipc_test(topology, pairs, iter, cores);
            // Execute function
            break;
        case 3: // Semaphore
            // Execute function
            break;
        case 4: // Mutex
            measure_time = pthread_test(topology, pairs, iter, cores);
            // Execute function
            break;
        default:
            break;
    }

    printf("measure_time: %f\n", measure_time);
    
    return 0;
}
