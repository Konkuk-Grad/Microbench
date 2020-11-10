#include "mbti_pthread.h"
#include "mbti_pthread_inter_spsc.h"
#include "mbti_pthread_spsc.h"
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

void* pthread_inter_create_pair(void* arg){
    pthread_t thread_id[2];
    
    if(pthread_create(&thread_id[0], NULL, pthread_inter_pair1,(void*)arg) < 0){
        perror("[pthread_spsc_thread_create]");
    }
    if(pthread_create(&thread_id[1], NULL, pthread_inter_pair2,(void*)arg) < 0){
        perror("[pthread_spsc_thread_create]");
    }
    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);
    fprintf(stderr,"hello\n");
    pthread_exit((void*)0);
}

void* pthread_inter_pair1(void* arg){
    int id = (int)arg;
    int iter = pthread_try_count;
    pthread_setaffinity(pthread_mask);
    clock_gettime(CLOCK_MONOTONIC, &p_msg.start_point);
    for(int i = 0; i < iter; i++){
        //ping
        pthread_mutex_lock(&pthread_lock);
        while(pthread_count == 1){
            pthread_cond_wait(&pthread_full,&pthread_lock);
        }
        strncpy(pthread_buf, "Hello pair2!", 12);
        pthread_count = 1;
        pthread_cond_signal(&pthread_empty);
        pthread_mutex_unlock(&pthread_lock);
        //pong
        pthread_mutex_lock(&pthread_lock2);
        while(pthread_count2 == 0){
            pthread_cond_wait(&pthread_empty2,&pthread_lock2);
        }
        fprintf(stderr,"Group[%d]/[%s] from pair2\n", id, pthread_buf);
        memset(pthread_buf,0,256);
        pthread_count2 = 0;
        pthread_mutex_unlock(&pthread_lock2);

    }
    clock_gettime(CLOCK_MONOTONIC, &p_msg.end_point);
    fprintf(stderr,"hello\n");
    pthread_exit((void*)0);
}

void* pthread_inter_pair2(void* arg){
    int id = (int)arg;
    int iter = pthread_try_count;
    pthread_setaffinity(pthread_mask);
    for(int i = 0; i < iter; i++){
        //ping
        pthread_mutex_lock(&pthread_lock);
        while(pthread_count == 0 ){
            pthread_cond_wait(&pthread_empty,&pthread_lock);
        }
        fprintf(stderr,"Group[%d]/[%s] from pair1\n", id, pthread_buf);
        memset(pthread_buf,0,256);
        pthread_count = 0;
        pthread_cond_signal(&pthread_full);
        pthread_mutex_unlock(&pthread_lock);

        //pong
        pthread_mutex_lock(&pthread_lock2);
        while(pthread_count2 == 1){
            pthread_cond_wait(&pthread_full2,&pthread_lock2);
        }
        strncpy(pthread_buf, "Hello pair1!", 12);
        pthread_count2 = 1;
        pthread_cond_signal(&pthread_empty2);
        pthread_mutex_unlock(&pthread_lock2);
    }
    pthread_exit((void*)0);
}