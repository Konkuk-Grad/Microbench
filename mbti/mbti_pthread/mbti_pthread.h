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
    #define PTHREAD_ATTR

    struct timespec *pthread_start_point, *pthread_end_point;
    unsigned int pthread_try_count;
    unsigned int pthread_thread_num;
    unsigned int pthread_ready_flag = 0;
    long pthread_ncores;
    pthread_mutex_t pthread_lock;
    pthread_mutex_t pthread_condition_lock;
    pthread_cond_t pthread_cond;

#endif
    double pthread_test(int topology, int processes, int iter, int num_cpus);
    void init_pthread();
    void return_result(pthread_t *p_thread);

