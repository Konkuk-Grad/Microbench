#include <time.h>
struct pthread_msg{
    struct timespec start_point;
    struct timespec end_point;
};
void pthread_spsc_thread_act();
void* pthread_producer(void* arg);
void* pthread_consumer(void* arg);