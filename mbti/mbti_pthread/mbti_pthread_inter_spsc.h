#include <pthread.h>
extern int inuse; 

void* pthread_inter_create_pair(void* arg);
void* pthread_inter_pair1(void* arg);
void* pthread_inter_pair2(void* arg);