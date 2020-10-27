/***** Parent Header *****/

#include "mbti_ipc.h"

// Start mbti_mq_pp.h
#ifndef __MBT_MQ_PP_H__
#define __MBT_MQ_PP_H__

/* ------- */
/* Defines */
/* ------- */

#define MQ_PP_NAME "/mq_pp"
#define MQ_MAX_MSG 10 // Count
#define MQ_MSG_SIZE 1 // Byte
#define MQ_PP_MAX_MSG 1025 // Count
#define MQ_PP_MSG_SIZE 8 // Byte
#define SHM_KEY_NUM 9527
#define SHM_MEM_SIZE 1024

/* ------- */
/* Headers */
/* ------- */

#include <sched.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* ---------------- */
/* Global Variables */
/* ---------------- */

int mq_pp_shmid;

/* --------- */
/* Functions */
/* --------- */

double mq_pingpong(int pairs, int iter, int cores);
double mq_ping(int num, int iter, int cores);
int mq_pong(int num, int iter, int cores);
void set_core_affinity(int cores);
double get_time_diff(unsigned int flag);
void shm_create();
void shm_write(long long *shareddata, int size);
void shm_read(long long *sMemory);
void shm_free();

#endif
