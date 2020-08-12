#include "global_attr.h"
#include "../mbti_pthread.h"

void increase_counter(int thread_id){
    pthread_mutex_lock(&pthread_lock);
    pthread_g_counter+=1;
    pthread_mutex_unlock(&pthread_lock);
}

void* pthread_global_thread_act(void* arg){
    unsigned int l_counter;
    int thread_id = (int)arg;
    pid_t tid;

    pthread_mutex_lock(&pthread_condition_lock);
    pthread_ready_flag+=1;
    while(pthread_ready_flag != pthread_thread_num){
        pthread_cond_wait(&pthread_cond, &pthread_condition_lock);
    }
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_condition_lock);

    clock_gettime(CLOCK_MONOTONIC, &pthread_start_point[thread_id]);
    for(l_counter = 0; l_counter < pthread_try_count;l_counter++){
        increase_counter(thread_id);
    }
    clock_gettime(CLOCK_MONOTONIC, &pthread_end_point[thread_id]);
    pthread_exit((void*)tid);
}