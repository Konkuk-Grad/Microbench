#define _GNU_SOURCE

/* --------- */
/*  Headers  */
/* --------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>
#include <sched.h>
#include <mqueue.h>
#include <sys/types.h>

/* --------- */
/*  Define   */
/* --------- */
#define MQ_FILE "/mbt_mq3"

/* ----------- */
/*  Structure  */
/* ----------- */

typedef struct __time_msg{
    int mtype;
    double measure_time;
}time_msg;


/* ------------------ */
/*  Global Variables  */
/* ------------------ */
struct timespec start_point, end_point;
double measure_time;
// pid_t ping_pid, pong_pid;

// int curr_iter_count = 0;
// int recv_pong_count = 0;
int complete_processes = 0;
int user_iter_count = 0;

/* --------- */
/* Functions */
/* --------- */

/* ------------------ */
/*  Global Variables  */
/* ------------------ */
pid_t *pid_arr;
pid_t ping_pid, pong_pid;

int curr_iter_count = 0;
int recv_ping_count = 0;

/* --------- */
/* Functions */
/* --------- */

/*** Topology ***/

// /** 1. Ping-pong **/
// pid_t* init_pingpong(int pairs, int iter, int num_cpus);

// /* 1) Ping Process */
// void recv_pong();
// void end_ping();

// /* Pong Process */
// void recv_ping();
// void end_pong();

/* 1) Ping Process */
void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point); // Individual checking time
    
    if(curr_iter_count++ < user_iter_count){
#ifdef DEBUGMSG
        printf("[DEBUG][I] I->O ping_count: %d\n", ping_count); // Send ping
#endif
        kill(pong_pid, SIGUSR1);        
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end_point);
#ifdef DEBUGMSG
        printf("[DEBUG][Terminate][I] I->O ping_count: %d\n", --curr_iter_count);
#endif
        kill(pong_pid, SIGUSR2); // Terminate pong process
    }
}

void end_ping(){
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(time_msg);
    time_msg msgbox;

    int mfd = -1, tmp = -1;
    if((mfd = mq_open(MQ_FILE, O_RDWR)) == -1){
        perror("[end_ping] open_error");
        exit(-1);
    }

    if(mq_receive(mfd, (char *)&msgbox, sizeof(time_msg), NULL) == -1){
        perror("[end_ping] recv error");
        exit(-1);
    } else {
        if(msgbox.measure_time == -complete_processes){
            kill(getppid(), SIGCONT);
        } else {
            msgbox.measure_time--;
            if(mq_send(mfd, (const char *)&msgbox, sizeof(time_msg), 0) == -1){
                perror("[end_ping] negative send error");
                exit(-1);
            }
        }

        kill(getpid(), SIGSTOP);

        msgbox.mtype = 0;
        msgbox.measure_time = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;

        if(mq_send(mfd, (const char *)&msgbox, sizeof(time_msg), 0) == -1){
            perror("[end_ping] send time error");
            exit(-1);
        }
    }

    // printf("[Total  ] {%f} ms\n", measure_time);
    // printf("[Average] {%f} ms/iter \n", measure_time / user_iter_count);

    exit(0);
}

/* 2) Pong Process */
void recv_ping(){
    if(recv_ping_count++ < user_iter_count){
        kill(ping_pid, SIGUSR1); // SEND PONG
#ifdef DEBUGMSG
        printf("[DEBUG][O] O->I recv_ping_count: %d\n", recv_ping_count); // Send pong
#endif
    } else {
        // dummy else
    }
}

void end_pong(){
#ifdef DEBUGMSG
    printf("[DEBUG][endpong][O] recv_ping_count: %d\n", recv_ping_count);
#endif
    kill(ping_pid, SIGUSR2);
    exit(0);    
}

pid_t* init_pingpong(int pairs, int iter, int num_cpus){

    sigset_t sigset; // Block SIGUSR1,2
    sigset_t oldset; // Prev sigset
    struct sigaction act[2]; // 0: SIGUSR1, 1: SIGUSR2
    cpu_set_t *cpu_mask;
    size_t size;
    int result;

    user_iter_count = iter;
    complete_processes = pairs;
    pid_arr = (pid_t *)malloc(sizeof(pid_t) * pairs);

    /* Set a number of iterations */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);

    /* Set cpu affinities */
    cpu_mask = CPU_ALLOC(num_cpus);
    if(cpu_mask == NULL){
        perror("CPU_ALLOC");
        exit(-1);
    }

    size = CPU_ALLOC_SIZE(num_cpus);
    CPU_ZERO_S(size, cpu_mask);

    for(int i = 0; i < num_cpus; i++){
        CPU_SET_S(i, size, cpu_mask);
    }
    
    for(int i = 0; i < pairs; i++){
        if(!(pid_arr[i] = fork())){ // Child (Ping)
            if(pong_pid = fork()){ // Parent (Pong)

                /* Setting Handler */            
                act[0].sa_handler = recv_pong;
                act[0].sa_flags = 0;

                act[1].sa_handler = end_ping;
                act[1].sa_flags = 0;

                sigaction(SIGUSR1, &act[0], NULL); // Received signal
                sigaction(SIGUSR2, &act[1], NULL); // End signal

                sigprocmask(SIG_BLOCK, &sigset, &oldset);

                /* Set cpu affinity */
                result = sched_setaffinity(0, sizeof(*cpu_mask), cpu_mask);
                if(result == -1){
                    printf("pid [%d] sched_setaffinity failed!\n", getpid());
                }            

                /* Ping-pong logic */
                kill(getpid(), SIGSTOP);
                clock_gettime(CLOCK_MONOTONIC, &start_point);
                recv_pong();
                while(1){
                    sigsuspend(&oldset);
                }
            } else { // Child (Ping)
                ping_pid = getppid();

                /* Setting Handler */
                act[0].sa_handler = recv_ping;
                act[0].sa_flags = 0;

                act[1].sa_handler = end_pong;
                act[1].sa_flags = 0;

                sigaction(SIGUSR1, &act[0], NULL);
                sigaction(SIGUSR2, &act[1], NULL);

                sigprocmask(SIG_BLOCK, &sigset, &oldset);

                /* Set cpu affinity */
                result = sched_setaffinity(0, sizeof(*cpu_mask), cpu_mask);
                if(result == -1){
                    printf("pid [%d] sched_setaffinity failed!\n", getpid());
                }         

                while(1){
                    sigsuspend(&oldset);
                }
            }
        } else {
            printf("pid: %d, fork: %d\n", getpid(), i);
        }
    }

    printf("hello world! %d\n", getpid());
    return pid_arr;
}



// Signal Testing Function
int sig_test(int topology, int processes, int iter, int num_cpus); // Signal Test Main
int sig_test_attr(); // Signal Test Setting Attribute
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus); // Signal Test Initialization
double sig_test_exec(pid_t *trig_pid, int processes, int topology); // Signal Test

int sig_test(int topology, int processes, int iter, int num_cpus){
    pid_t* trig_pid = NULL;
    trig_pid = sig_test_init(topology, processes, iter, num_cpus);

    printf("processes: %d, trig_pid = %p\n", processes, trig_pid);

    for(int i = 0; i < processes; i++){
        printf("trig_pid[%d]: %d\n", i, trig_pid[i]);
    }
    sig_test_exec(trig_pid, processes, topology);

    free(trig_pid);

}
int sig_test_attr(); // Signal Test Setting Attribute
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus){ // Signal Test Initialization
    switch(topology){
        case 1: // Ping-pong
            printf("hello i'm ping-pong\n");
            pid_t* tmp = NULL;
            printf("[b] tmp: %p\n", tmp);
            tmp = init_pingpong(processes, iter, num_cpus);
            printf("[a] tmp: %p\n", tmp);
            return tmp;
        default:
            return NULL;
    }

    return NULL;
} 
double sig_test_exec(pid_t *trig_pid, int processes, int topology){ // Signal Test
    mqd_t mfd = -1;
    struct mq_attr msgqattr;
    int tmp = -1;
    time_msg msgbox;

    double* measure_times;
    measure_times = (double *)malloc(sizeof(double) * processes);

    printf("MQ_FILE: %s, getpid(): %d\n", MQ_FILE, getpid());

    sleep(5);
    msgqattr.mq_flags = 0;
    msgqattr.mq_maxmsg = 1;
    msgqattr.mq_msgsize = sizeof(time_msg);
    msgqattr.mq_curmsgs = 0;

    if((mfd = mq_open(MQ_FILE, O_RDWR | O_CREAT, 0666, &msgqattr)) == -1){
        perror("open_error");
        exit(-1);
    }

    msgbox.mtype = 0;
    msgbox.measure_time = -1;

    if(mq_send(mfd, (const char *)&msgbox, msgqattr.mq_msgsize, 1) == -1){
        perror("[main] send error");
        return -1;
    }

    // Start Tests
    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    kill(getpid(), SIGSTOP);

    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    for(int i = 0; i < processes; i++){
        if(mq_receive(mfd, (char *)&msgbox, msgqattr.mq_msgsize, NULL) == -1){
            perror("receive error");
        } else {
            measure_times[i] = msgbox.measure_time;
            printf("measure_times[%d]: %f\n", i, measure_times[i]);
        }
    }

    double sum = 0;
    switch(topology){
        case 1: // Ping-pong
            
            for(int i = 0; i < processes; i++){
                sum += measure_times[i];
            }

            free(measure_times);
            return sum / processes;
        default:
            break;
    }

    return 0;
} 


