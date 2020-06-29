#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>

struct timeval start_point, end_point;
double measure = 0;
pid_t pid1, pid2;

void send_ping(){
    gettimeofday(&start_point, NULL);
    kill(pid2, SIGUSR1);
}

void recv_ping(){
    kill(pid1, SIGUSR1); // SEND PONG
}

void recv_pong(){
    gettimeofday(&end_point, NULL);
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

    measure = (end_point.tv_sec - start_point.tv_sec) * 1000000 + end_point.tv_usec - start_point.tv_usec;
    measure /= 1000;

    printf("[P] measure time: %f ms\n", measure);

    return 0;
}