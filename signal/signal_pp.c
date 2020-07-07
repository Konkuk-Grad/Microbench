#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#define BUF_SIZE 1

struct timespec start_point, end_point;

double measure = 0;
pid_t pid1, pid2;

void send_ping(){
    // clock_gettime(CLOCK_MONOTONIC, &start_point);
    kill(pid2, SIGUSR1);
}

void recv_ping(){
    kill(pid1, SIGUSR1); // SEND PONG
}

void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point);
}

int main(int argc, char *argv[]){

    int iter;

    if(argc != 2){
        printf("[Error] argc must be 2! (Input: %d)\n", argc);
        return -1;
    }

    if(!atoi(argv[1])){
        printf("[Error] argv[1] must be positive integer (Input: %s)\n", argv[1]);
        return -1;
    }

    iter = atoi(argv[1]);

    if(!(pid2 = fork())){ // Child
        pid1 = getppid(); // Parent PID
        pid2 = getpid(); // Child PID
#ifdef DEBUGMSG
        printf("[C] Parent PID: %d, Child PID: %d\n", pid1, pid2);
#endif
        signal(SIGUSR1, recv_ping);
        while(1){ // Until getting SIGKILL, Infinity loop
            pause();
        }
        exit(0);
    } else { // Parent
        pid1 = getpid(); // Parent PID
        signal(SIGUSR1, recv_pong);
#ifdef DEBUGMSG
        printf("[P] Parent PID: %d, Child PID: %d\n", pid1, pid2);
#endif
        sleep(3); // Wait until setting a child process
        // Measure a total execution time
        clock_gettime(CLOCK_MONOTONIC, &start_point);
        for(int i = 0; i < iter; i++){
            send_ping(); // Send pong to a child process
            pause(); // Wait until receiving pong from a child process
            // measure += (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;
        }
        clock_gettime(CLOCK_MONOTONIC, &end_point);
    }

    // Calculate a total execution time
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;
    // measure /= iter;

#ifdef DEBUGMSG
    printf("[P] (pid: %d) measure time\n", getpid());
#endif
    printf("[Total  ] %f ms\n", measure);
    printf("[Average] %f ms\n", measure / iter);

    return 0;
}