#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#ifdef CLOCK_GETTIME
struct timespec start_point, end_point;
#else
struct timeval start_point, end_point;
#endif

double measure = 0;
pid_t pid1, pid2;

void send_ping(){
#ifdef CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &start_point);
#else
    gettimeofday(&start_point, NULL);
#endif
    kill(pid2, SIGUSR1);
}

void recv_ping(){
    kill(pid1, SIGUSR1); // SEND PONG
}

void recv_pong(){
#ifdef CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &end_point);
#else
    gettimeofday(&end_point, NULL);
#endif
}

int main(int argc, char *argv[]){

    if(!(pid2 = fork())){ // Child
        pid1 = getppid(); // Parent
        pid2 = getpid(); // Child
        printf("[C] Parent PID: %d, Child PID: %d\n", pid1, pid2);
        signal(SIGUSR1, recv_ping);
        pause();
        exit(0);
    } else { // Parent
        pid1 = getpid();
        signal(SIGUSR1, recv_pong);
        printf("[P] Parent PID: %d, Child PID: %d\n", pid1, pid2);
        sleep(3);
        send_ping();
        pause();
    }
#ifdef CLOCK_GETTIME
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000000000 + end_point.tv_nsec - start_point.tv_nsec;
    measure /= 1000000;
#else
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000000 + end_point.tv_usec - start_point.tv_usec;
    measure /= 1000;
#endif
    printf("[P] measure time: %f ms\n", measure);

    return 0;
}