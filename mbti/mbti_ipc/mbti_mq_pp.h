/***** Parent Header *****/

#include "mbti_ipc.h"

// Start mbti_mq_pp.h
#ifndef __MBT_MQ_PP_H__
#define __MBT_MQ_PP_H__

/* ------- */
/* Defines */
/* ------- */

#define MQ_MAX_MSG 10 // Count
#define MQ_MSG_SIZE 4 // Byte

/* ------- */
/* Headers */
/* ------- */

#include <sched.h>

/* ---------------- */
/* Global Variables */
/* ---------------- */

/* --------- */
/* Functions */
/* --------- */

int mq_pingpong(int pairs, int iter, int cores);
int mq_ping(int num, int iter);
int mq_pong(int num, int iter);
void set_core_affinity(int cores);
long long get_time_diff(unsigned int flag);

#endif
