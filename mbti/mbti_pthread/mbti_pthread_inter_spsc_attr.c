#include "mbti_pthread_inter_spsc.h"
#include <pthread.h>

pthread_mutex_t pthread_lock3;
int inuse = -1; 