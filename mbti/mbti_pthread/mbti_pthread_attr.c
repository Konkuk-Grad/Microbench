#include <pthread.h>
#include "mbti_pthread.h"
    struct timespec *pthread_start_point, *pthread_end_point;
    unsigned int pthread_try_count;
    unsigned int pthread_thread_num;
    unsigned int pthread_ready_flag=0;
    long pthread_ncores;
    pthread_mutex_t pthread_lock;
    pthread_mutex_t pthread_condition_lock;
    pthread_cond_t pthread_cond;