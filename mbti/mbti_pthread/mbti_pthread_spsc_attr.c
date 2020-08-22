#include "mbti_pthread_spsc.h"
#include <pthread.h>

int pthread_full = 0, pthread_full2 = 0;
char pthread_buf[256];
struct pthread_msg p_msg;
pthread_cond_t pthread_empty;
pthread_cond_t pthread_pthread_full;
pthread_cond_t pthread_empty2;
pthread_cond_t pthread_pthread_full2;