#ifndef PTHREAD_GLOBAL
    #define PTHREAD_GLOBAL 1
    extern unsigned int pthread_g_counter;

    void* pthread_global_thread_act(void* arg);
    void increase_counter(int thread_id);
#endif