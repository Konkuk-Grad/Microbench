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
struct msgbuf{
    long msgtype;
    // long measure_time;
    pid_t pid;
};

double measure = 0;
pid_t pid1, pid2;

void recv_ping(){
    kill(pid1, SIGUSR1); // SEND PONG
}

int main(int argc, char *argv[]){

    key_t key_id;
    struct msgbuf pingbuf;

    if(argc != 3){
        printf("[Pong] argc was %d, argc must be 2!\n", argc);
        printf("[Pong] ./pong (Message Queue Key) (Iter)\n");
        exit(0);
    }

    if(!atoi(argv[1]) || !atoi(argv[2])){
        printf("[Pong] argv[1] or [2] is not a positive number (%s, %s)\n", argv[1], argv[2]);
        exit(0);
    }

    int iter = atoi(argv[2]);
    
    key_id = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    if(key_id == -1){
        perror("[Pong] msgget error: ");
        exit(0);
    }

    printf("[Pong] Key: %d\n", key_id);

    pingbuf.msgtype = 2;
    // pingbuf.measure_time = 0;
    pingbuf.pid = getpid();
    
    if(msgsnd(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 0) == -1){
        perror("[Pong] msgsnd error: ");
        exit(0);
    }

    memset(&pingbuf, 0, sizeof(struct msgbuf));

    if(msgrcv(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 1, 0) == -1){
        perror("[Pong] msgrcv error");
        exit(0);
    }

    pid1 = pingbuf.pid; // Pong
    pid2 = getpid(); // Pong
    printf("[Pong] Ping PID: %d, Pong PID: %d\n", pid1, pid2);

    signal(SIGUSR1, recv_ping);
    while(1){
        // printf("[Pong] wait for what?\n");
        pause();
    }

    return 0;
}