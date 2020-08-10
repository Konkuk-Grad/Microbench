#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <pthread.h>


/* Global Variables */
struct timespec *start_point, *end_point;

double measure = 0;
pid_t pid1, pid2;

int ping_count = 0;
int pong_count = 0;

int iter = 0;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t global_cond = PTHREAD_COND_INITIALIZER;

/* Functions */
/* Ping Process [I] */

static void *ping(){
    
}

// static void *ping(){
//     // Measure a total execution time
// #ifdef PPDEBUG
//     printf("[DEBUG][START][I] I->O ping_count: %d\n", ++ping_count); // Send ping
// #endif
//     ping_count++;
//     kill(pid2, SIGUSR1);
}

static void pong(){
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
    int pairs = 0; // Number of processes' pairs
    sigset_t sigset; // Block SIGUSR1
    sigset_t oldset; // Prev sigset
    struct sigaction act[2]; // 0: SIGCONT / 1: SIGUSR1

    if(argc != 3){
        printf("[Error] argc must be 2! (Input: %d)\n", argc);
        return -1;
    }

    if(atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0){
        printf("[Error] argv[1] and argv[2] must be positive integer (Input: %s, %s)\n", argv[1], argv[2]);
        return -1;
    }

    /* Set a number of iterations */
    iter = atoi(argv[1]);
    local_iter = iter;

    /* Set a number of processes' pairs */
    pairs = atoi(argv[2]);

    /* Set a sigset blocking SIGUSR1 */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    pid_t *ping_pid = (pid_t *)malloc(sizeof(pid_t) * pairs);
    pid_t *pong_pid = (pid_t *)malloc(sizeof(pid_t) * pairs);

    start_point = (struct timespec *)malloc(sizeof(struct timespec) * pairs);
    end_point = (struct timespec *)malloc(sizeof(struct timespec) * pairs);

    for(int i = 0; i < pairs; i++){
        pthread_create(&ping_pid[i], NULL, ping, NULL);
        pthread_create(&pong_pid[i], NULL, pong, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &start_point);
    pthread_cond_broadcast(&global_cond);
    pthread_mutex_destroy(&global_mutex);

    for(int i = 0; i < pairs; i++){
        pthread_join(&ping_pid[i], NULL);
        pthread_join(&pong_pid[i], NULL);
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