#include "mbti.h"

int main(int argc, char *argv[]){

    int topology, pairs, iter, cores;

    /* 
    * 1. Topology
    * 2. Number of pairs
    * 3. Number of iterations
    * 4. Number of cores
    */

    if(argc != 5){
        printf("[ERROR] argc is must be 5 (Input: %d)\n", argc);
        return -1;
    }

    for(int i = 1; i < argc; i++){
        if(!atoi(argv[i])){
            printf("[Error] argv[%d] must be positive integer (Input: %s)\n", i, argv[i]);
            return -1;
        }
    }

    topology = atoi(argv[1]);
    pairs = atoi(argv[2]);
    iter = atoi(argv[3]);
    cores = atoi(argv[4]);

    double measure_time = 0;
    switch(topology){
        case 1: // Ping-pong
            // measure_time = sig_test(topology, pairs, iter, cores);
            break;
        case 2: // MUTEX
            measure_time = pthread_test(topology, pairs, iter, cores);
        default:
            break;
    }

    printf("measure_time: %f\n", measure_time);
    
    return 0;
}