#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "mutex_lock.h"


// Define
struct timespec *start_point, *end_point;



// Global Variables
double measure = 0;
int iter = 10, curr = 0; // Default Iteration is 10
pid_t pid1, pid2;


int producer_counter = 0;

/* Signal handlers */
// void sigcontHandler(){ return ; }
// void send_ping(){
// #ifdef CLOCK_GETTIME
//     clock_gettime(CLOCK_REALTIME, &start_point[curr]);
// #else
//     gettimeofday(&start_point[curr], NULL);
// #endif
//     kill(pid2, SIGUSR1);
// }
void produce(){
    struct msgbuf* buf;
    buf = init_buf(producer_counter++);

}
void consume(long count){

}


void recv_pong(){
#ifdef CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &end_point[curr]);
#else
    gettimeofday(&end_point[curr], NULL);
#endif

#ifdef PPDEBUG
    printf("[DEBUG][pid: %d] (recv_pong) curr (%d)\n", pid1, curr);
#endif
}

// int main(int argc, char *argv[]){

//     key_t key_id;
//     struct msgbuf pingbuf;

//     if(argc != 3){
//         printf("[Ping] argc was %d, argc must be 3!\n", argc);
//         printf("[Ping] ./ping (Message Queue Key) (Iter)\n");
//         exit(0);
//     }

//     if(!atoi(argv[1]) || !atoi(argv[2])){
//         printf("[Ping] argv[1] or [2] is not a positive number (%s, %s)\n", argv[1], argv[2]);
//         exit(0);
//     }

//     iter = atoi(argv[2]);

//     /* 1. Exchange pids between ping and pong */
//     key_id = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
//     if(key_id == -1){
//         perror("[Ping] ping-pong msgget error: ");
//         exit(0);
//     }

//     printf("[Ping] Key: %d\n", key_id);

//     pingbuf.msgtype = 1;
//     pingbuf.measure_time = 0;
//     pingbuf.pid = getpid();
    
// #ifdef CLOCK_GETTIME
//     start_point = (struct timespec *)malloc(sizeof(struct timespec) * iter);
//     end_point   = (struct timespec *)malloc(sizeof(struct timespec) * iter);
// #else
//     start_point = (struct timeval *)malloc(sizeof(struct timeval) * iter);
//     end_point = (struct timeval *)malloc(sizeof(struct timeval) * iter);
// #endif

//     if(msgsnd(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 0) == -1){
//         perror("[Ping] msgsnd error: ");
//         exit(0);
//     }

//     memset(&pingbuf, 0, sizeof(struct msgbuf));

//     if(msgrcv(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 2, 0) == -1){
//         perror("[Ping] msgrcv error: ");
//         exit(0);
//     }

//     pid1 = getpid(); // Send Ping PID
//     pid2 = pingbuf.pid; // Received Pong PID
//     printf("[Ping] Ping PID: %d, Pong PID: %d\n", pid1, pid2);

//     /* 2. Send ping's pid to control process */
//     // key_id = msgget((key_t)111222333, IPC_CREAT | 0666);
//     // if(key_id == -1){
//     //     perror("[Ping] ctl msgget error: ");
//     //     exit(0);
//     // }

//     // pingbuf.msgtype = 10;
//     // pingbuf.pid = getpid();
//     // pingbuf.measure_time = 0;

//     // printf("[Ping] send to ctl\n");
//     // if(msgsnd(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 0) == -1){
//     //     perror("[Ping] ctl msgsnd error: ");
//     //     exit(0);
//     // }

//     // printf("[Ping] send complete!\n");

//     /* 3. Setting handler and some settings */
//     signal(SIGUSR1, recv_pong); // Setting up signal handler
//     // signal(SIGCONT, sigcontHandler); // Setting up signal handler

//     /* 4. Wait SIGCONT from control process */
//     sleep(5); // Wait for creating pong process
//     // pause(); // Wait until SIGCONT from control process
//     printf("[Ping] (pid: %d) Get SIGCONT\n", pid1);

//     /* 5. Start Ping-Pong */
//     for(curr = 0; curr < iter; curr++){
//         // sleep(2); // For avoid race condition
//         send_ping();
//         pause();
//     }

//     /* 6. Calculate measure time */
//     for(curr = 0; curr < iter; curr++){
// #ifdef CLOCK_GETTIME
//         // measure += (double)((end_point[curr].tv_sec - start_point[curr].tv_sec) * 1000000000 + end_point[curr].tv_nsec - start_point[curr].tv_nsec) / 1000000;
//         measure += ((end_point[curr].tv_sec - start_point[curr].tv_sec) * 1000 + (double)(end_point[curr].tv_nsec - start_point[curr].tv_nsec) / 1000000);
// #else
//         measure += (double)((end_point[curr].tv_sec - start_point[curr].tv_sec) * 1000000 + end_point[curr].tv_usec - start_point[curr].tv_usec) / 1000;
// #endif
//     }

//     measure /= iter; // Average

//     /* 7. Send to a control process */
//     // memset(&pingbuf, 0 ,sizeof(struct msgbuf));
//     // pingbuf.msgtype = 12;
//     // pingbuf.pid = pid1;
//     // pingbuf.measure_time = measure;
//     // if(msgsnd(key_id, (void *)&pingbuf, sizeof(struct msgbuf), 0) == -1){
//     //     perror("[Ping] ctl msgsnd error: ");
//     //     exit(0);
//     // }

// #ifdef PPDEBUG
//     printf("[DEBUG] PING pid (%d)\n", pid1);
//     for(int i = 0; i < iter; i++){
// #ifdef CLOCK_GETTIME
//         printf("[DEBUG] [pid: %d] [%d]: %f ms\n", pid1, i, (double)((end_point[i].tv_sec - start_point[i].tv_sec) * 1000000000 + end_point[i].tv_nsec - start_point[i].tv_nsec) / 1000000);
// #else
//         printf("[DEBUG] [pid: %d] [%d]: %f ms\n", pid1, i, (double)((end_point[i].tv_sec - start_point[i].tv_sec) * 1000000 + end_point[i].tv_usec - start_point[i].tv_usec) / 1000);
// #endif
//     }
// #endif

//     printf("[Ping] measure time: %f ms\n", measure);

//     /* 7. Exit process */
//     free(end_point);
//     free(start_point);

//     sleep(2);
//     kill(pid2, SIGKILL); // Kill Pong Process
//     return 0;
// }
