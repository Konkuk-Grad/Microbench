#include "mbti_pthread.h"
#include "mbti_pthread_inter_spsc.h"
#include "mbti_pthread_spsc.h"
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

void* pthread_inter_create_pair(void* arg){
    pthread_t thread_id[2];
    pthread_cond_init(&pthread_empty,NULL);
    pthread_cond_init(&pthread_full,NULL);
    pthread_cond_init(&pthread_empty2,NULL);
    pthread_cond_init(&pthread_full2,NULL);
    
    pthread_mutex_init(&pthread_lock3, NULL);

    if(pthread_create(&thread_id[0], NULL, pthread_inter_pair1,(void*)pthread_try_count) < 0){
        perror("[pthread_spsc_thread_create]");
    }
    if(pthread_create(&thread_id[1], NULL, pthread_inter_pair2,(void*)pthread_try_count) < 0){
        perror("[pthread_spsc_thread_create]");
    }
    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);
}

void* pthread_inter_pair1(void* arg){
    int id = (int)arg;
    int iter = pthread_try_count;
    pthread_setaffinity();
    clock_gettime(CLOCK_MONOTONIC, &p_msg.start_point);
    for(int i = 0; i < iter; i++){
        //ping
        pthread_mutex_lock(&pthread_lock);
        while(pthread_count == 1){
            pthread_cond_wait(&pthread_full,&pthread_lock);
        }
        pthread_mutex_lock(&pthread_lock3);
        inuse = id;
        strncpy(pthread_buf, "Hello pair2!", 12);
        pthread_count = 1;
        pthread_cond_signal(&pthread_empty);
        pthread_mutex_unlock(&pthread_lock);
        //pong
        pthread_mutex_lock(&pthread_lock2);
        while(pthread_count2 == 0 && id != inuse){
            pthread_cond_wait(&pthread_empty2,&pthread_lock2);
        }
        fprintf(stderr,"Group[%d]/[%s] from pair2\n", id, pthread_buf);
        memset(pthread_buf,0,256);
        pthread_count2 = 0;
        inuse = -1;
        pthread_mutex_unlock(&pthread_lock3);
        
        pthread_cond_signal(&pthread_full2);
        pthread_mutex_unlock(&pthread_lock2);

    }
    clock_gettime(CLOCK_MONOTONIC, &p_msg.end_point);
}

void* pthread_inter_pair2(void* arg){
    int id = (int)arg;
    int iter = pthread_try_count;
    pthread_setaffinity();
    for(int i = 0; i < iter; i++){
        inuse = id;
        //ping
        while(pthread_count == 0 && id != inuse){
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
        inuse = id;
        strncpy(pthread_buf, "Hello pair1!", 12);
        pthread_count2 = 1;
        pthread_cond_signal(&pthread_empty2);
        pthread_mutex_unlock(&pthread_lock2);
    }
}