#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// Define
#ifdef CLOCK_GETTIME
struct timespec *start_point, *end_point;
#else
struct timeval *start_point, *end_point;
#endif

struct msgbuf{
    long msgtype;
    pid_t pid;
};


double measure = 0;
int iter = 10, curr = 0; // Default Iteration is 10
pid_t pid1, pid2;

void send_ping(){
#ifdef CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &start_point[curr]);
#else
    gettimeofday(&start_point[curr], NULL);
#endif
    kill(pid2, SIGUSR1);
}

void recv_ping(){
    kill(pid1, SIGUSR1); // SEND PONG
}

void recv_pong(){
#ifdef CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &end_point[curr]);
#else
    gettimeofday(&end_point, NULL);
#endif
}

int main(int argc, char *argv[]){

    key_t key_id;
    struct msgbuf pingbuf;

    if(argc != 3){
        printf("[Ping] argc was %d, argc must be 2!\n", argc);
        printf("[Ping] ./ping (Message Queue Key) (Iter)\n");
        exit(0);
    }

    if(!atoi(argv[1]) || !atoi(argv[2])){
        printf("[Ping] argv[1] or [2] is not a positive number (%s, %s)\n", argv[1], argv[2]);
        exit(0);
    }

    iter = atoi(argv[2]);

    key_id = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    if(key_id == -1){
        perror("[Ping] msgget error: ");
        exit(0);
    }

    printf("[Ping] Key: %d\n", key_id);

    pingbuf.msgtype = 1;
    pingbuf.pid = getpid();
    
#ifdef CLOCK_GETTIME
    start_point = (struct timespec *)malloc(sizeof(struct timespec) * iter);
    end_point   = (struct timespec *)malloc(sizeof(struct timespec) * iter);
#else
    start_point = (struct timeval *)malloc(sizeof(struct timeval) * iter);
    end_point = (struct timeval *)malloc(sizeof(struct timeval) * iter);
#endif

    if(msgsnd(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 0) == -1){
        perror("[Ping] msgsnd error: ");
        exit(0);
    }

    memset(&pingbuf, 0, sizeof(struct msgbuf));

    if(msgrcv(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 2, 0) == -1){
        perror("[Ping] msgrcv error: ");
        exit(0);
    }

    pid1 = getpid(); // Send Ping PID
    pid2 = pingbuf.pid; // Received Pong PID
    printf("[Ping] Ping PID: %d, Pong PID: %d\n", pid1, pid2);

    signal(SIGUSR1, recv_pong); // Setting signal handler
    for(curr = 0; curr < iter; curr++){
        sleep(2);
        send_ping();
        pause();
        
#ifdef CLOCK_GETTIME
        measure += (double)((end_point[curr].tv_sec - start_point[curr].tv_sec) * 1000000000 + end_point[curr].tv_nsec - start_point[curr].tv_nsec) / 1000000;
#else
        measure += (double)((end_point[curr].tv_sec - start_point[curr].tv_sec) * 1000000 + end_point[curr].tv_usec - start_point[curr].tv_usec) / 1000;
#endif
    }

    measure /= iter;
    printf("[Ping] measure time: %f ms\n", measure);

    free(end_point);
    free(start_point);
    return 0;
}