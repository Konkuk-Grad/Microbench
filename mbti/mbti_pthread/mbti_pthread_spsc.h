#include <time.h>
#include <pthread.h>
struct pthread_msg{
    struct timespec start_point;
    struct timespec end_point;
};
extern char pthread_buf[256];
extern int pthread_count, pthread_count2;
extern struct pthread_msg p_msg;
void pthread_create_pair();
void pthread_spsc_thread_act();
void* pthread_pair1(void* arg);
void* pthread_pair2(void* arg);