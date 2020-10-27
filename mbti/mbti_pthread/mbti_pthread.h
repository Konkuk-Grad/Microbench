#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#ifndef PTHREAD_ATTR
    #define PTHREAD_ATTR 1
    extern cpu_set_t pthread_mask;
    extern struct timespec *pthread_start_point, *pthread_end_point;
    extern unsigned int pthread_try_count;
    extern unsigned int pthread_thread_num;
    extern unsigned int pthread_ready_flag;
    extern long pthread_ncores;
    extern pthread_mutex_t pthread_lock;
    extern pthread_mutex_t pthread_lock2;
    extern pthread_mutex_t pthread_glob_lock;
    extern pthread_mutex_t pthread_condition_lock;
    extern pthread_cond_t pthread_cond;

//for SPSC
    extern pthread_cond_t pthread_empty;
    extern pthread_cond_t pthread_full;
    extern pthread_cond_t pthread_empty2;
    extern pthread_cond_t pthread_full2;

    double pthread_test(int topology, int processes, int iter, int num_cpus);
    void init_pthread();
    double return_result();
    void pthread_setaffinity();
#endif

