#include "mbti_pthread.h"
#include "mbti_pthread_spsc.h"
#include "mbti_pthread_global.h"
#include <fcntl.h>
#include <mqueue.h>

typedef void (*func_type)();
/* This function is for initilize all variable which is used for later use */ 
void init_pthread(){
    /* initilization for pthread_mutex & pthread_cond */
    pthread_mutex_init(&pthread_lock, NULL);
    pthread_mutex_init(&pthread_lock2,NULL);
    pthread_mutex_init(&pthread_glob_lock, NULL);
    pthread_mutex_init(&pthread_condition_lock, NULL);
    pthread_cond_init(&pthread_cond, NULL);
    pthread_cond_init(&pthread_empty,NULL);
    pthread_cond_init(&pthread_full,NULL);
    pthread_cond_init(&pthread_empty2,NULL);
    pthread_cond_init(&pthread_full2,NULL);

    /* initilization for timespec */
    pthread_start_point = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    pthread_end_point   = (struct timespec*)malloc(sizeof(struct timespec)* pthread_thread_num);
    if( !pthread_start_point || !pthread_end_point){
        PRINTERROR("[Pthread]init_pthread failed\n");
        exit(1);
    }else{
        DEBUGMSG("start_point addr : %x, end_point addr : %x\n", pthread_start_point, pthread_end_point);
    }
}
/* This function free variables which use malloc */
void exit_pthread(){
    free(pthread_start_point);
    pthread_start_point = NULL;
    free(pthread_end_point);
    pthread_end_point =NULL;
    if(!pthread_end_point || !pthread_start_point){
        DEBUGMSG("Free Done\n");
    }else{
        PRINTERROR("[Pthread]exit_pthread failed\n");
        exit(1);
    }
}
/* This function is from calculate time spent */
double return_result(){
    double measure = 0;
    for(int i = 0; i < pthread_thread_num; i++){
        measure += ((pthread_end_point[i].tv_sec - pthread_start_point[i].tv_sec) * 1000 + 
        (double)(pthread_end_point[i].tv_nsec - pthread_start_point[i].tv_nsec) / 1000000);
    }
    DEBUGMSG("measure = %f\n",measure);

    return measure/pthread_thread_num;
}
/* Setting affinity to core */
void pthread_setaffinity(cpu_set_t mask){
    pthread_t current_thread = pthread_self();
    if(mask.__bits == 0){
        PRINTERROR("[Pthread] mask = %d\n",mask);
        exit(1);
    }
    if(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &mask)){
        PRINTERROR("[Pthread]pthread_setaffinity_np failed\n");
        exit(0);
    }
    DEBUGMSG("Set affinity Done\n");
}
void create_pthread(void* thread_func){
    pthread_t *p_thread = (pthread_t*)malloc(sizeof(pthread_t)*pthread_thread_num);
    int pthread_id;
    int status;
    for(int i = 0; i < pthread_thread_num; i++){
        pthread_id = pthread_create(&p_thread[i], NULL, thread_func, (void*)i);
        if (pthread_id < 0){
            PRINTERROR("[Pthread]pthread_create failed\n");
            exit(0);
        }
    }
    DEBUGMSG("%d Threads Generated\n", pthread_thread_num);
    for(int i = 0; i < pthread_thread_num; i++){
            pthread_join(p_thread[i],(void**)&status);
    }
    free(p_thread); 
}
/* This function is technical main function for pthread test case */
/* 1. Allocate global variable from argc, argv from main function */
/* 2. Set CPU affinity                                            */ 
/* 3. set func pointer for thread_act                             */
/* 4. Create threads in #processes variable                       */
/* 5. Call return_result                                          */
/* 6. Call exit_pthread                                           */
double pthread_test(int topology, int processes, int iter, int num_cpus){
    int cpu = 0;
    void* thread_func;
    double res = 0;

    if(topology < 0){
        PRINTERROR("Topology : %d\n", topology);
        exit(1);
    }else if (processes < 0){
        PRINTERROR("processes Num : %d\n",processes);
        exit(1);
    }else if (iter < 0){
        PRINTERROR("Iteration : %d\n",iter);
        exit(1);
    }else if (num_cpus < 0){
        PRINTERROR("CPU Num : %d\n",num_cpus);
        exit(1);
    }


    //1. 
    pthread_try_count = iter;
    pthread_thread_num = processes;
    init_pthread();

    //2.
    CPU_ZERO(&pthread_mask);
    for(int i = 0; i < num_cpus; i++){
        CPU_SET(i, &pthread_mask);
    }
    DEBUGMSG("Input Value Set\n");
    //3.
    switch (topology)
    {
    case 1:
        thread_func = pthread_global_thread_act;
        break;
    /* For later use */
    case 2:
        thread_func = pthread_create_pair;
        break;
    // case 3:
        // thread_func = pthread_inter_create_pair;
        // break;
    default:
        thread_func = pthread_global_thread_act;
        break;
    }

    //4. 
    if(topology != 2){
        create_pthread(thread_func);
    } else {
        ((void(*)())thread_func)();
    }
    DEBUGMSG("Pthread TASK Done\n");
   //5.
    res = return_result();
    exit_pthread();
    
    return res; 
}
