#include "mbti_pthread.h"
#include "mbti_pthread_spsc.h"
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

void pthread_create_pair(){
    int pid;
    struct mq_attr attr;
    attr.mq_maxmsg = pthread_thread_num;
    attr.mq_msgsize = sizeof(struct pthread_msg);
    struct pthread_msg msg;
    mqd_t mfd;
    mfd = mq_open("/mbti_pthread", O_RDWR | O_CREAT, 0666, &attr);

    if(mfd == -1){
        perror("[msgget]");
        exit(0);
    }
    for(int i = 0; i < pthread_thread_num; i++){
        pid = fork();
        if(pid == 0){
            pthread_spsc_thread_act();
            exit(0);
        } else if(pid <0 ){
            perror("[fork]");
        }
    }
    if(pid != 0){
        for(int i = 0; i < pthread_thread_num; i++){
            if((mq_receive(mfd, (char*)&msg,attr.mq_msgsize,NULL)) == -1){
                exit(-1);
            }
            pthread_start_point[i] = msg.start_point;
            pthread_end_point[i] = msg.end_point;
        }
        // for(int i = 0; i < pthread_thread_num; i++){
        //    wait(&status);
        // } 
    }
}

void pthread_spsc_thread_act(){

    pthread_t thread_id[2];
    struct mq_attr attr;
    attr.mq_maxmsg = pthread_thread_num;
    attr.mq_msgsize = sizeof(struct pthread_msg);
    mqd_t mfd;
    mfd = mq_open("/mbti_pthread", O_WRONLY, 0666, &attr);
    if(mfd == -1){
        perror("[mq_open]");
        exit(0);
    }
    pthread_cond_init(&pthread_empty,NULL);
    pthread_cond_init(&pthread_pthread_full,NULL);
    pthread_cond_init(&pthread_empty2,NULL);
    pthread_cond_init(&pthread_pthread_full2,NULL);
    
    if(pthread_create(&thread_id[0], NULL, pthread_producer,(void*)pthread_try_count) < 0){
        perror("[pthread_spsc_thread_create]");
    }
    if(pthread_create(&thread_id[1], NULL, pthread_consumer,(void*)pthread_try_count) < 0){
        perror("[pthread_spsc_thread_create]");
    }

    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);
    if((mq_send(mfd, (char*)&p_msg, attr.mq_msgsize, 1)) == -1){
        perror("[mq_send]");
        exit(-1);
    }
}

void* pthread_producer(void* arg){
    int iter = (int)arg;
    clock_gettime(CLOCK_MONOTONIC, &p_msg.start_point);
    for(int i = 0; i < iter; i++){
        //producing
        pthread_mutex_lock(&pthread_lock);
        while(pthread_full == 1){
            pthread_cond_wait(&pthread_pthread_full,&pthread_lock);
        }
        strncpy(pthread_buf, "Hello pair2!", 12);
        pthread_full = 1;
        pthread_cond_signal(&pthread_empty);
        pthread_mutex_unlock(&pthread_lock);

        pthread_mutex_lock(&pthread_lock2);
        while(pthread_full2 == 0){
            pthread_cond_wait(&pthread_empty2,&pthread_lock2);
        }
        fprintf(stderr,"[%s] from pair2\n", pthread_buf);
        memset(pthread_buf,0,256);
        pthread_full2 = 0;
        pthread_cond_signal(&pthread_pthread_full2);
        pthread_mutex_unlock(&pthread_lock2);
    }
    clock_gettime(CLOCK_MONOTONIC, &p_msg.end_point);
}

void* pthread_consumer(void* arg){
    int iter = (int)arg;
    for(int i = 0; i < iter; i++){
        //consume
        pthread_mutex_lock(&pthread_lock);
        while(pthread_full == 0){
            pthread_cond_wait(&pthread_empty,&pthread_lock);
        }
        fprintf(stderr,"[%s] from pair1\n", pthread_buf);
        memset(pthread_buf,0,256);
        pthread_full = 0;
        pthread_cond_signal(&pthread_pthread_full);
        pthread_mutex_unlock(&pthread_lock);


        //produce
        pthread_mutex_lock(&pthread_lock2);
        while(pthread_full2 == 1){
            pthread_cond_wait(&pthread_pthread_full2,&pthread_lock2);
        }
        strncpy(pthread_buf, "Hello pair1!", 12);
        pthread_full2 = 1;
        pthread_cond_signal(&pthread_empty2);
        pthread_mutex_unlock(&pthread_lock2);
        
    }
}