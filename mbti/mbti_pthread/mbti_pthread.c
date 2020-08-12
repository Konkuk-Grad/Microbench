#include "global/global_attr.h"
#include "mbti_pthread.h"

/* This function is for initilize all variable which is used for later use */ 
void init_pthread(){
    /* initilization for pthread_mutex & pthread_cond */
    pthread_mutex_init(&pthread_lock, NULL);
    pthread_mutex_init(&pthread_condition_lock, NULL);
    pthread_cond_init(&pthread_cond, NULL);
    /* initilization for timespec */
    pthread_start_point = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    pthread_end_point   = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
}
/* This function free variables which use malloc */
void exit_pthread(){
    free(pthread_start_point);
    free(pthread_end_point);
}
/* This function is from calculate time spent */
void return_result(pthread_t *p_thread){
    double measure = 0;
    int status;
    for(int i = 0; i < pthread_thread_num; i++){
        pthread_join(p_thread[i],(void**)&status);
        measure = ((pthread_end_point[i].tv_sec - pthread_start_point[i].tv_sec) * 1000 + 
        (double)(pthread_end_point[i].tv_nsec - pthread_start_point[i].tv_nsec) / 1000000);
        fprintf(stdout,"{%7d}\n", measure);
    }
}
/* This function is technical main function for pthread test case */
/* 1. Allocate global variable from argc, argv from main function */
/* 2. Set CPU affinity                                            */ 
/* 3. set func pointer for thread_act                             */
/* 4. Create threads in #processes variable                       */
/* 5. Call return_result                                          */
/* 6. Call exit_pthread                                           */
double pthread_test(int topology, int processes, int iter, int num_cpus){
    pthread_t *p_thread;
    pid_t pid;
    cpu_set_t mask;
    int pthread_id;
    int cpu = 0;
    void* thread_func;
    //1. 
    pthread_try_count = iter;
    pthread_thread_num = processes;
    init_pthread();

    //2.
    CPU_ZERO(&mask);
    for(int i = 0; i < num_cpus; i++){
        CPU_SET(i, &mask);
    }
    if(sched_setaffinity(pid, sizeof(mask),&mask)){
        exit(1);
    }

    //3.
    switch (topology)
    {
    case 1:
        thread_func = pthread_global_thread_act;
        break;
    /* For later use */
    case 2:
        // thread_func = pthread_elses_blabla
    default:
        break;
    }

    //4. 
    p_thread = (pthread_t*)malloc(sizeof(pthread_t)*pthread_thread_num);
    for(int i = 0; i < pthread_thread_num; i++){
        // response_time[i] = 0;
        pthread_id = pthread_create(&p_thread[i], NULL, thread_func, (void*)i);
        if (pthread_id < 0){
            perror("[pthread_create]");
            exit(0);
        }
    }
    //5.
    return_result(p_thread);
    exit_pthread();
    free(p_thread);
    return 0;
}