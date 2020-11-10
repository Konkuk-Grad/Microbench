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
            DEBUGMSG("Processes Generated %d",getpid());
            pthread_spsc_thread_act();
            exit(0);
        } else if(pid <0 ){
            PRINTERROR("[Pthread] fork failed");
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
    
    if(pthread_create(&thread_id[0], NULL, pthread_pair1,(void*)pthread_try_count) < 0){
        PRINTERROR("[Pthread]pthread_create failed");
        perror("[pthread_spsc_thread_create]");
    }
    DEBUGMSG("pthread producer Generated\n");
    if(pthread_create(&thread_id[1], NULL, pthread_pair2,(void*)pthread_try_count) < 0){
        PRINTERROR("[Pthread]pthread_create failed");
        perror("[pthread_spsc_thread_create]");
    }
    DEBUGMSG("pthread comsumer Generated\n");

    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);
    if((mq_send(mfd, (char*)&p_msg, attr.mq_msgsize, 1)) == -1){
        perror("[mq_send]");
        exit(-1);
    }
}

void* pthread_pair1(void* arg){
    int iter = (int)arg;
    char local = 1;
    pthread_setaffinity(pthread_mask);
    clock_gettime(CLOCK_MONOTONIC, &p_msg.start_point);
    for(int i = 0; i < iter; i++){
        //producing
        pthread_mutex_lock(&pthread_lock);
        while(pthread_count == 1){
            pthread_cond_wait(&pthread_full,&pthread_lock);
        }
        pthread_buf = local;
        // strncpy(pthread_buf, "Hello pair2!", 12);
        DEBUGMSG("Critical Section : pair1 push");
        pthread_count = 1;
        pthread_cond_signal(&pthread_empty);
        pthread_mutex_unlock(&pthread_lock);

        pthread_mutex_lock(&pthread_lock2);
        while(pthread_count2 == 0){
            pthread_cond_wait(&pthread_empty2,&pthread_lock2);
        }
        DEBUGMSG("Critical Section : pair1 pull");
        // fprintf(stderr,"[%s] from pair2\n", pthread_buf);
        // memset(pthread_buf,0,256);
        local = pthread_buf;
        pthread_count2 = 0;
        pthread_cond_signal(&pthread_full2);
        pthread_mutex_unlock(&pthread_lock2);
    }
    clock_gettime(CLOCK_MONOTONIC, &p_msg.end_point);
}

void* pthread_pair2(void* arg){
    int iter = (int)arg;
    char local = 0;
    pthread_setaffinity(pthread_mask);
    for(int i = 0; i < iter; i++){
        //consume
        pthread_mutex_lock(&pthread_lock);
        while(pthread_count == 0){
            pthread_cond_wait(&pthread_empty,&pthread_lock);
        }
        DEBUGMSG("Critical Section : pair2 push");
        // fprintf(stderr,"[%s] from pair1\n", pthread_buf);
        // memset(pthread_buf,0,256);
        // pthread_buf = 0;
        local = pthread_buf;
        pthread_count = 0;
        pthread_cond_signal(&pthread_full);
        pthread_mutex_unlock(&pthread_lock);


        //produce
        pthread_mutex_lock(&pthread_lock2);
        while(pthread_count2 == 1){
            pthread_cond_wait(&pthread_full2,&pthread_lock2);
        }
        DEBUGMSG("Critical Section : pair2 pull");
        // strncpy(pthread_buf, "Hello pair1!", 12);
        pthread_buf = local;
        pthread_count2 = 1;
        pthread_cond_signal(&pthread_empty2);
        pthread_mutex_unlock(&pthread_lock2);
        
    }
}