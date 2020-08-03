#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>

/* Global Variables */
struct timespec start_point, end_point;

double measure = 0;
pid_t pid1, pid2;

int ping_count = 0;
int pong_count = 0;

int iter = 0;

/* Functions */
/* Ping Process [I] */
void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point); // Individual checking time
    
    if(ping_count++ < iter){
#ifdef DEBUGMSG
        printf("[DEBUG][I] I->O ping_count: %d\n", ping_count); // Send ping
#endif
        kill(pid2, SIGUSR1);        
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end_point);
        printf("[DEBUG][Terminate][I] I->O ping_count: %d\n", ping_count); // Send ping
        ping_count--;
        kill(pid2, SIGUSR2); // Terminate pong process
    }
}

// Terminate Ping Process
void end_ping(){
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;

    printf("[Total  ] {%f} ms\n", measure);
    printf("[Average] {%f} ms\n", measure / iter);

    exit(0);
}

/* Pong Process [O] */
void recv_ping(){
    if(pong_count++ < iter){
        kill(pid1, SIGUSR1); // SEND PONG
#ifdef DEBUGMSG
        printf("[DEBUG][O] O->I pong_count: %d\n", pong_count); // Send pong
#endif
    } else {
        // dummy else
    }
}

// Terminate Pong Process
void end_pong(){
#ifdef DEBUGMSG
    printf("[DEBUG][endpong][O] pong_count: %d\n", pong_count);
#endif
    kill(pid1, SIGUSR2);
    exit(0);
}


/* Main function */
int main(int argc, char *argv[]){

    sigset_t sigset; // Block SIGUSR1
    sigset_t oldset; // Prev sigset
    struct sigaction act[2]; // 0: SIGUSR1 / 1: SIGUSR2
    cpu_set_t mask;
    
// int result = sched_setaffinity(0, sizeof(mask), &mask);

    if(argc != 4){
        printf("[Error] argc must be 2! (Input: %d)\n", argc);
        return -1;
    }

    if(!atoi(argv[1]) || !atoi(argv[2]) || !atoi(argv[3])){
        printf("[Error] argv must be positive integer (Input: %s, %s, %s)\n", argv[1], argv[2], argv[3]);
        return -1;
    }

    /* Set a number of iterations */
    iter = atoi(argv[1]);

    /* Set a sigset blocking SIGUSR1 */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);

    /* Set cpu affinities */
    CPU_ZERO(&mask);

    if(!(pid2 = fork())){ // Child
        pid1 = getppid(); // Parent PID
        pid2 = getpid(); // Child PID

        /* Setting Handler */
        act[0].sa_handler = recv_ping;
        act[0].sa_flags = 0;

        act[1].sa_handler = end_pong;
        act[1].sa_flags = 0;

        sigaction(SIGUSR1, &act[0], NULL);
        sigaction(SIGUSR2, &act[1], NULL);

        sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1

        /* Setting cpu affinity */
        CPU_SET(atoi(argv[3]), &mask);
        int result = sched_setaffinity(0, sizeof(mask), &mask);
        if(result == -1){
            printf("pid [%d] sched_setaffinity failed!\n", getpid());
        }

        while(1){ // Until getting SIGKILL, Infinity loop
            sigsuspend(&oldset); // Temporarily unblock SIGUSR1 and wait
        }
    } else { // Parent
        pid1 = getpid(); // Parent PID
        printf("{%d,%d}\n", pid1, pid2); // Parsing PIDs {Ping, Pong}

        /* Setting Handler */
        act[0].sa_handler = recv_pong;
        act[0].sa_flags = 0;

        act[1].sa_handler = end_ping;
        act[1].sa_flags = 0;
        
        sigaction(SIGUSR1, &act[0], NULL); // Received Signal
        sigaction(SIGUSR2, &act[1], NULL); // End Signal

        sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1, SIGUSR2

        /* Setting cpu affinity */
        CPU_SET(atoi(argv[2]), &mask);
        int result = sched_setaffinity(0, sizeof(mask), &mask);
        if(result == -1){
            printf("pid [%d] sched_setaffinity failed!\n", getpid());
        }

        // pause(); // Wait until getting SIGCONT
        kill(getpid(), SIGSTOP);
        clock_gettime(CLOCK_MONOTONIC, &start_point);
        recv_pong();
        while(1){
            sigsuspend(&oldset); // Temporarily unblock SIGUSR1 and wait
        }
    }

    return 0;
}
