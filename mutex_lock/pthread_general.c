#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct timespec *start_point, *end_point;
unsigned int g_counter;
unsigned int try_count;
unsigned int thread_num;
unsigned int ready_flag = 0;

pthread_mutex_t lock;
pthread_mutex_t condition_lock;

pthread_cond_t cond;

void increase_counter(int thread_id){
    pthread_mutex_lock(&lock);
    g_counter+=1;
    // fprintf(stderr,"[thread_%2d]g_counter : %d ** \n", thread_id, g_counter);
    pthread_mutex_unlock(&lock);
}

void* thread_act(void* arg){
    unsigned int l_counter;
    int thread_id = (int)arg;
    
    pthread_mutex_lock(&condition_lock);
    ready_flag+=1;
    while(ready_flag != thread_num){
        pthread_cond_wait(&cond, &condition_lock);
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&condition_lock);


    clock_gettime(CLOCK_MONOTONIC, &start_point[thread_id]);
    for(l_counter = 0; l_counter < try_count;l_counter++){
        increase_counter(thread_id);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_point[thread_id]);
    // fprintf(stderr, "debg[%d] : %lf %lf\n",thread_id,(double)start_point[thread_id].tv_sec, (double)start_point[thread_id].tv_nsec);
}

int main(int argc,char* argv[]){
    pthread_t *p_thread;
    int pthread_id;
    int status;
    double measure = 0;
    if(argc!= 3){
        printf("usage : ./pthread_general [$numOfThread] [tryCount]");
        exit(0);
    }

    if(!(atoi(argv[1]) && atoi(argv[2]))){
        printf("[ERROR] argv[1] or [2] is not a positive number (%s, %s)\n", argv[1], argv[2]);
        exit(0);
    }

    try_count = atoi(argv[2]);
    thread_num = atoi(argv[1]);

    //initilization
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&condition_lock, NULL);

    pthread_cond_init(&cond, NULL);
    start_point = (struct timespec*)malloc(sizeof(struct timespec)* thread_num);
    end_point   = (struct timespec*)malloc(sizeof(struct timespec)* thread_num);
    p_thread = (pthread_t*)malloc(sizeof(pthread_t)*thread_num);
    
    for(int i = 0; i < thread_num; i++){
        pthread_id = pthread_create(&p_thread[i], NULL, thread_act, (void*)i);
        if (pthread_id < 0){
            perror("[pthread_create]");
            exit(0);
        }
    }

    for(int i = 0; i < thread_num; i++){
        pthread_join(p_thread[i],(void**)&status);
        measure = ((end_point[i].tv_sec - start_point[i].tv_sec) * 1000 + (double)(end_point[i].tv_nsec - start_point[i].tv_nsec) / 1000000);
        fprintf(stderr,"Thread %3d is Ended with %10lf status : %d\n", i,measure,status);
    }
    if(g_counter != try_count * thread_num){
        printf("g_counter race conditon");
    }

    return 0;
}

