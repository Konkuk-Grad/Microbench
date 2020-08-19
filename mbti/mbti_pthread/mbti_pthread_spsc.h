#include <time.h>
#include <pthread.h>
struct pthread_msg{
    struct timespec start_point;
    struct timespec end_point;
};
extern char pthread_buf[256];
extern int pthread_full, pthread_full2;
extern struct pthread_msg p_msg;
extern pthread_cond_t pthread_empty;
extern pthread_cond_t pthread_pthread_full;
extern pthread_cond_t pthread_empty2;
extern pthread_cond_t pthread_pthread_full2;
void pthread_create_pair();
void pthread_spsc_thread_act();
void* pthread_producer(void* arg);
void* pthread_consumer(void* arg);