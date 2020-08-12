#include "global_attr.h"
#include "../mbti_pthread.h"

void increase_counter(int thread_id){
    pthread_mutex_lock(&pthread_lock);
    // clock_gettime(CLOCK_MONOTONIC, &run_point[thread_id]);
    // response_time[thread_id] += ((run_point[thread_id].tv_sec - arrive_point[thread_id].tv_sec) * 1000 + (double)(run_point[thread_id].tv_nsec - arrive_point[thread_id].tv_nsec) / 1000000);
    pthread_g_counter+=1;
    pthread_mutex_unlock(&pthread_lock);
}

void* pthread_global_thread_act(void* arg){
    unsigned int l_counter;
    int thread_id = (int)arg;
    int cpu = 0;
    pid_t tid;
    cpu_set_t mask;
    tid = syscall(SYS_gettid);

    pthread_mutex_lock(&pthread_condition_lock);
    pthread_ready_flag+=1;
    while(pthread_ready_flag != pthread_thread_num){
        pthread_cond_wait(&pthread_cond, &pthread_condition_lock);
    }
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_condition_lock);

    clock_gettime(CLOCK_MONOTONIC, &pthread_start_point[thread_id]);
    for(l_counter = 0; l_counter < pthread_try_count;l_counter++){
        // clock_gettime(CLOCK_MONOTONIC, &arrive_point[thread_id]);
        increase_counter(thread_id);
    }
    clock_gettime(CLOCK_MONOTONIC, &pthread_end_point[thread_id]);
    pthread_exit((void*)tid);
}