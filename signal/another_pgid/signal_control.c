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

struct msgbuf{
    long msgtype;
    pid_t pid;
    long measure_time;
};

int main(int argc, char *argv[]){
    int i, iter;
    pid_t *pid_arr;
    key_t key_id;
    struct msgbuf tmpbuf;
    double measure_time = 0;

    if(argc != 2){
        printf("[CTL] argc was %d, argc must be 2!\n", argc);
        printf("[Ping] ./ping (Number of processes)\n");
        exit(0);
    }

    if(!atoi(argv[1])){
        printf("[Ping] argv[1] is not a positive number (%s)\n", argv[1]);
        exit(0);
    }

    iter = atoi(argv[1]);
    pid_arr = (pid_t *)malloc(sizeof(pid_t) * iter);

    key_id = msgget((key_t)111222333, IPC_CREAT | 0666);
    if(key_id == -1){
        perror("[CTL] msgget error: ");
        exit(0);
    }

    for(i = 0; i < iter; i++){
        if(msgrcv(key_id, (void *)&tmpbuf, sizeof(struct msgbuf), 10, 0) == -1){
            perror("[CTL] msgrcv error: ");
            exit(0);
        }

        pid_arr[i] = tmpbuf.pid;
    }

    printf("[CTL] Get number of pids: %d\n", i);

    sleep(5);
    for(i = iter-1; i >= 0; i--){
        printf("[CTL] kill pid_arr[%d]: %d, SIGCONT\n", i, pid_arr[i]);
        kill(pid_arr[i], SIGCONT);
    }

    for(i = 0; i < iter; i++){
        memset(&tmpbuf, 0, sizeof(struct msgbuf));
        if(msgrcv(key_id, (void *)&tmpbuf, sizeof(struct msgbuf), 12, 0) == -1){
            perror("[CTL] gettime msgrcv error: ");
            exit(0);
        }

        measure_time += tmpbuf.measure_time;
    }

    measure_time /= iter;

    printf("[CTL] measure_time: %f\n", measure_time);

    free(pid_arr);
    return 0;
}