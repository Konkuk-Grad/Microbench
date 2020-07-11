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

struct timespec start_point, end_point;

double measure = 0;
pid_t pid1, pid2;

int ping_count = 0;
int pong_count = 0;

/* Ping Process [I] */
void send_ping(sigset_t *set, sigset_t *old){
    // clock_gettime(CLOCK_MONOTONIC, &start_point); // Individual checking time
#ifdef PPDEBUG
    printf("[DEBUG][I] I->O ping_count: %d\n", ++ping_count); // Send ping
#endif    
    kill(pid2, SIGUSR1);
}

void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point); // Individual checking time
#ifdef PPDEBUG
    printf("[DEBUG][I] O->I pong_count: %d\n\n", ++pong_count); // Receive pong
#endif
}

/* Pong Process [O] */
void recv_ping(){
#ifdef PPDEBUG
    printf("[DEBUG][O] I->O ping_count: %d\n", ++ping_count); // Receive ping
    printf("[DEBUG][O] O->I pong_count: %d\n", ++pong_count); // Send pong
#endif    
    kill(pid1, SIGUSR1); // SEND PONG
}


/* Main function */
int main(int argc, char *argv[]){

    int iter; // Number of iterations
    sigset_t sigset; // Block SIGUSR1
    sigset_t oldset; // Prev sigset
    struct sigaction act;

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
        act.sa_handler = recv_ping;
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);

        while(1){ // Until getting SIGKILL, Infinity loop
            pause();
        }
        exit(0);
    } else { // Parent
        pid1 = getpid(); // Parent PID

        /* Setting Handler */
        act.sa_handler = recv_pong;
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);


#ifdef DEBUGMSG
        printf("[I] Parent PID: %d, Child PID: %d\n", pid1, pid2);
#endif
        sleep(3); // Wait until setting a child process => Will be deprecated
        // pause(); // Wait until getting SIGCONT
        sigprocmask(SIG_BLOCK, &sigset, &oldset);

        // Measure a total execution time
        clock_gettime(CLOCK_MONOTONIC, &start_point);
        for(int i = 0; i < iter; i++){
            send_ping(&sigset, &oldset); // Send pong to a child process
            sigsuspend(&oldset); // Temporarily unblock SIGUSR1 and wait
        }
        clock_gettime(CLOCK_MONOTONIC, &end_point);

        kill(SIGKILL, pid2);
    }

    // Calculate a total execution time
    measure = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;

#ifdef DEBUGMSG
    printf("[P] (pid: %d) measure time\n", getpid());
#endif
    printf("[Total  ] {%f} ms\n", measure);
    printf("[Average] {%f} ms\n", measure / iter);

    return 0;
}