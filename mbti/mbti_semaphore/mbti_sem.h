/***** Parent Header *****/
#include "../mbti.h"

// Start mbti_sem.h

/* ------- */
/* Headers */
/* ------- */
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
/* ---------------- */
/* Global Variables */
/* ---------------- */
static sem_t sem_empty, sem_full, sem_mutex;
//버퍼가 비었는지, 찼는지, 버퍼에 접근 가능한지에 대한 semaphore변수
int sem_buffer[1024], sem_front, sem_rear,shm_id;
//scsp 상황에서 공유되는 버퍼, 버퍼의 앞 과 뒤를 가르키는 변수, 공유메모리 id
int sem_user_iter,sem_user_processes;
//유저가 입력한 반복 횟수, 유저가 입력한 프로세스 개수
struct timespec sem_begin, sem_end;
//시간 표시를 위한 시작 시각, 마치는 시각
void* shm_addr;
//공유 메모리 주소

/* ------- */
/* Defines */
/* ------- */
#define shm_key 3
#define shm_size 1024

/* --------- */
/* Functions */
/* --------- */
int sem_put_item();
int sem_consume_item();
void* sem_producer(void* arg);
void* sem_consumer(void* arg);
double sem_iter_exec(int iter,int num_cpus);
double sem_make_processes(int processes,int iter,int num_cpus);
void sem_set_core_affinities(int num_cpus);
void make_shm();