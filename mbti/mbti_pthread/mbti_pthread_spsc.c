#include "mbti_pthread.h"
#include "mbti_pthread_spsc.h"
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

int full, full2;
struct pthread_msg p_msg;
pthread_cond_t pthread_empty;
pthread_cond_t pthread_full;
pthread_cond_t pthread_empty2;
pthread_cond_t pthread_full2;

pthread_mutex_t pthread_lock2;

char buf[256];
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
    struct mq_attr attr;
    attr.mq_maxmsg = pthread_thread_num;
    attr.mq_msgsize = sizeof(struct pthread_msg);
    mqd_t mfd;
    mfd = mq_open("/mbti_pthread", O_WRONLY, 0666, &attr);
    if(mfd == -1){
        perror("[mq_open]");
        exit(0);
    }
    full = full2 = 0;
    pthread_t thread_id[2];
    pthread_mutex_init(&pthread_lock2,NULL);
    pthread_cond_init(&pthread_empty,NULL);
    pthread_cond_init(&pthread_full,NULL);
    
    pthread_cond_init(&pthread_empty2,NULL);
    pthread_cond_init(&pthread_full2,NULL);
    
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
        while(full == 1){
            pthread_cond_wait(&pthread_full,&pthread_lock);
        }
        strncpy(buf, "Hello pair2!", 12);
        full = 1;
        pthread_cond_signal(&pthread_empty);
        pthread_mutex_unlock(&pthread_lock);

        pthread_mutex_lock(&pthread_lock2);
        while(full2 == 0){
            pthread_cond_wait(&pthread_empty2,&pthread_lock2);
        }
        fprintf(stderr,"[%s] from pair2\n", buf);
        memset(buf,0,256);
        full2 = 0;
        pthread_cond_signal(&pthread_full2);
        pthread_mutex_unlock(&pthread_lock2);
    }
    clock_gettime(CLOCK_MONOTONIC, &p_msg.end_point);
}

void* pthread_consumer(void* arg){
    int iter = (int)arg;
    for(int i = 0; i < iter; i++){
        //consume
        pthread_mutex_lock(&pthread_lock);
        while(full == 0){
            pthread_cond_wait(&pthread_empty,&pthread_lock);
        }
        fprintf(stderr,"[%s] from pair1\n", buf);
        memset(buf,0,256);
        full = 0;
        pthread_cond_signal(&pthread_full);
        pthread_mutex_unlock(&pthread_lock);


        //produce
        pthread_mutex_lock(&pthread_lock2);
        while(full2 == 1){
            pthread_cond_wait(&pthread_full2,&pthread_lock2);
        }
        strncpy(buf, "Hello pair1!", 12);
        full2 = 1;
        pthread_cond_signal(&pthread_empty2);
        pthread_mutex_unlock(&pthread_lock2);
        
    }
}