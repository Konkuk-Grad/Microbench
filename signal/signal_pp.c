#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#define NOT_WORKING 0
#define WORKING 1

/* Global Variables */
struct timespec start_point, end_point;

double measure = 0;
pid_t pid1, pid2;

int ping_count = 0;
int pong_count = 0;

int iter = 0;

/* Functions */
/* Ping Process [I] */
void start_ping(){
    // Measure a total execution time
#ifdef PPDEBUG
    printf("[DEBUG][START][I] I->O ping_count: %d\n", ++ping_count); // Send ping
#endif
    ping_count++;
    clock_gettime(CLOCK_MONOTONIC, &start_point);
    kill(pid2, SIGUSR1);
}

void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point); // Individual checking time
#ifdef PPDEBUG
    printf("[DEBUG][I] O->I pong_count: %d\n\n", ++pong_count); // Receive pong
    printf("[DEBUG][I] I->O ping_count: %d\n", ++ping_count); // Send ping
#endif
    ping_count++;
    kill(pid2, SIGUSR1);        
}

/* Pong Process [O] */
void recv_ping(){
#ifdef PPDEBUG
    printf("[DEBUG][O] I->O ping_count: %d\n", ++ping_count); // Receive ping
    printf("[DEBUG][O] O->I pong_count: %d\n", ++pong_count); // Send pong
#endif  
    pong_count++;
    kill(pid1, SIGUSR1); // SEND PONG
}


/* Main function */
int main(int argc, char *argv[]){

    int local_iter = 0; // Number of iterations
    sigset_t sigset; // Block SIGUSR1
    sigset_t oldset; // Prev sigset
    struct sigaction act[2]; // 0: SIGCONT / 1: SIGUSR1

    if(argc != 2){
        printf("[Error] argc must be 2! (Input: %d)\n", argc);
        return -1;
    }

    if(!atoi(argv[1])){
        printf("[Error] argv[1] must be positive integer (Input: %s)\n", argv[1]);
        return -1;
    }

    /* Set a number of iterations */
    iter = atoi(argv[1]);
    local_iter = iter;

    /* Set a sigset blocking SIGUSR1 */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    if(!(pid2 = fork())){ // Child
        pid1 = getppid(); // Parent PID
        pid2 = getpid(); // Child PID
#ifdef DEBUGMSG
        printf("[O] Parent PID: %d, Child PID: %d\n", pid1, pid2);
#endif
        /* Setting Handler */
        act[1].sa_handler = recv_ping;
        act[1].sa_flags = 0;

        sigaction(SIGUSR1, &act[1], NULL);

        sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1
        while(local_iter--){ // Until getting SIGKILL, Infinity loop
            sigsuspend(&oldset); // Temporarily unblock SIGUSR1 and wait
            // pause();
        }
        printf("[O] pong_count: %d\n", pong_count);
        exit(0);
    } else { // Parent
        pid1 = getpid(); // Parent PID

        /* Setting Handler */
        act[0].sa_handler = start_ping;
        act[0].sa_flags = 0;

        act[1].sa_handler = recv_pong;
        act[1].sa_flags = 0;
        
        sigaction(SIGCONT, &act[0], NULL);
        sigaction(SIGUSR1, &act[1], NULL);

        printf("{%d,%d}\n", pid1, pid2);
#ifdef DEBUGMSG
        printf("[I] Parent PID: %d, Child PID: %d\n", pid1, pid2);
#endif
        sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1
        pause(); // Wait until getting SIGCONT
        
        while(local_iter--){
            sigsuspend(&oldset); // Temporarily unblock SIGUSR1 and wait
            // pause();
        }
        clock_gettime(CLOCK_MONOTONIC, &end_point);
        ping_count--;
        kill(SIGKILL, pid2);
    }

    printf("[I] ping_count: %d\n", ping_count);
    // Calculate a total execution time
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;

#ifdef DEBUGMSG
    printf("[P] (pid: %d) measure time\n", getpid());
#endif
    printf("[Total  ] {%f} ms\n", measure);
    printf("[Average] {%f} ms\n", measure / iter);

    return 0;
}