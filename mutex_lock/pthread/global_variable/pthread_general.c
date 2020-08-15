#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>


struct timespec *start_point, *end_point;
struct timespec *arrive_point, *run_point;
double * response_time;
unsigned int g_counter;
unsigned int try_count;
unsigned int thread_num;
unsigned int ready_flag = 0;
long ncores;

pthread_mutex_t lock;
pthread_mutex_t condition_lock;
pthread_cond_t cond;


void increase_counter(int thread_id){

    trace_write("mutex_start");
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &run_point[thread_id]);
    response_time[thread_id] += ((run_point[thread_id].tv_sec - arrive_point[thread_id].tv_sec) * 1000 + (double)(run_point[thread_id].tv_nsec - arrive_point[thread_id].tv_nsec) / 1000000);
    g_counter+=1;
    pthread_mutex_unlock(&lock);
    trace_write("mutex_stop");

}

void* thread_act(void* arg){
    unsigned int l_counter;
    int thread_id = (int)arg;
    int cpu = 0;
    pid_t tid;
    cpu_set_t mask;
    tid = syscall(SYS_gettid);

#ifdef core
    cpu = thread_id%ncores+1;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);

    if(sched_setaffinity(tid, sizeof(mask),&mask)){
        exit(1);
    }
    if(sched_setaffinity(tid, sizeof(mask), &mask))
        exit(1);

#endif

    pthread_mutex_lock(&condition_lock);
    ready_flag+=1;
    while(ready_flag != thread_num){
        pthread_cond_wait(&cond, &condition_lock);
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&condition_lock);

    clock_gettime(CLOCK_MONOTONIC, &start_point[thread_id]);
    for(l_counter = 0; l_counter < try_count;l_counter++){
        clock_gettime(CLOCK_MONOTONIC, &arrive_point[thread_id]);
        increase_counter(thread_id);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_point[thread_id]);
    pthread_exit((void*)tid);
}

int main(int argc,char* argv[]){
    pthread_t *p_thread;
    int pthread_id;
    int status;
    double measure = 0;
    pid_t tid;
    cpu_set_t mask;
    struct sched_param param;


    printf("pid : %d\n", getpid());


#ifdef core
    ncores = sysconf(_SC_NPROCESSORS_ONLN)-1;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);    
    tid = syscall(SYS_gettid);
#endif
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
    arrive_point = (struct timespec*)malloc(sizeof(struct timespec)* thread_num);
    run_point = (struct timespec*)malloc(sizeof(struct timespec)* thread_num);
    response_time = (double *)malloc(sizeof(double)* thread_num); 
    
    p_thread = (pthread_t*)malloc(sizeof(pthread_t)*thread_num);
    
    for(int i = 0; i < thread_num; i++){
        response_time[i] = 0;
        pthread_id = pthread_create(&p_thread[i], NULL, thread_act, (void*)i);
        if (pthread_id < 0){
            perror("[pthread_create]");
            exit(0);
        }
    }

    for(int i = 0; i < thread_num; i++){
        pthread_join(p_thread[i],(void**)&status);
        measure = ((end_point[i].tv_sec - start_point[i].tv_sec) * 1000 + (double)(end_point[i].tv_nsec - start_point[i].tv_nsec) / 1000000);
        fprintf(stdout,"Thread %3d is Ended with %10lf status : %d aver response time : %10lf realrunning time : %10lf critical Entry ratio : %10lf\n", i,measure,status, response_time[i]/try_count, measure - response_time[i],response_time[i]/measure*100);
    }
    if(g_counter != try_count * thread_num){
        printf("g_counter race conditon");
    }
    return 0;
}