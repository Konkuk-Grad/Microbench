#include "mbti_pthread_spsc.h"
#include <pthread.h>

int pthread_count = 0, pthread_count2 = 0;
char pthread_buf[256];
struct pthread_msg p_msg;