/***** Parent Header *****/
//#include "../mbti.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Start mbti_signal.h

/* ------- */
/* Headers */
/* ------- */
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>

/* ---------------- */
/* Global Variables */
/* ---------------- */
static sem_t sem_empty, sem_full, sem_mutex;
//버퍼가 비었는지, 찼는지, 버퍼에 접근 가능한지에 대한 semaphore변수
int sem_buffer[1], sem_front, sem_rear;
//scsp 상황에서 공유되는 버퍼, 버퍼의 앞 과 뒤를 가르키는 변수들
int sem_user_iter,sem_user_processes;
//유저가 입력한 반복 횟수, 유저가 입력한 프로세스 개수
struct timespec sem_begin, sem_end;
//시간 표시를 위한 시작 시각, 마치는 시각

/* --------- */
/* Functions */
/* --------- */
int sem_put_item();
int sem_consume_item();
void* sem_producer(void* arg);
void* sem_consumer(void* arg);
double sem_test_exec(int topology, int processes, int iter, int num_cpus);