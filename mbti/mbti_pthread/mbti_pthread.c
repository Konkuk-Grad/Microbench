#include "global/global_attr.h"
#include "mbti_pthread.h"

double pthread_test(int topology, int processes, int iter, int num_cpus){
    
    pthread_t *p_thread;
    int pthread_id;
    int status;
    double measure = 0;
    pid_t tid;
    cpu_set_t mask;
    struct sched_param param;


    printf("pid : %d\n", getpid());

    pthread_try_count = iter;
    pthread_thread_num = processes;

    //initilization
    pthread_mutex_init(&pthread_lock, NULL);
    pthread_mutex_init(&pthread_condition_lock, NULL);
    pthread_cond_init(&pthread_cond, NULL);
    
    pthread_start_point = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    pthread_end_point   = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    // arrive_point = (struct timespec*)malloc(sizeof(struct timespec* pthread_thread_num);
    // run_point = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    // response_time = (double *)malloc(sizeof(double)* pthread_thread_num); 
    
    p_thread = (pthread_t*)malloc(sizeof(pthread_t)*pthread_thread_num);
    
    for(int i = 0; i < pthread_thread_num; i++){
        // response_time[i] = 0;
        pthread_id = pthread_create(&p_thread[i], NULL, pthread_global_thread_act, (void*)i);
        if (pthread_id < 0){
            perror("[pthread_create]");
            exit(0);
        }
    }

    for(int i = 0; i < pthread_thread_num; i++){
        pthread_join(p_thread[i],(void**)&status);
        measure = ((pthread_end_point[i].tv_sec - pthread_start_point[i].tv_sec) * 1000 + (double)(pthread_end_point[i].tv_nsec - pthread_start_point[i].tv_nsec) / 1000000);
        fprintf(stdout,"Thread %3d is Ended with %10lf status : %d aver response time : %10lf realrunning time : %10lf critical Entry ratio : %10lf\n", i,measure,status, response_time[i]/try_count, measure - response_time[i],response_time[i]/measure*100);
    }
    // if(g_counter != try_count * pthread_thread_num){
    //     printf("g_counter race conditon");
    // }
    return 0;
}