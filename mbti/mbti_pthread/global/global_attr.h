#ifndef PTHREAD_GLOBAL
    #define PTHREAD_GLOBAL
    unsigned int pthread_g_counter;
#endif
void* pthread_global_thread_act(void* arg);
void increase_counter(int thread_id)