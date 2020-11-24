/***** Parent Header *****/
#include "mbti_signal.h"

// Start mbti_sig_pp.h
#ifndef __MBT_SIG_PP_H__
#define __MBT_SIG_PP_H__

/* ------- */
/* Defines */
/* ------- */

/* ------- */
/* Headers */
/* ------- */
#include <sched.h>

/* ---------------- */
/* Global Variables */
/* ---------------- */
int curr_iter_count; // 현재 반복 횟수 (Ping, Pong)
int recv_ping_count; // Pong가 현재 받은 ping의 개수 (Pong)

pid_t *pid_arr; // Ping process들의 pid (Main)
pid_t ping_pid, pong_pid; // Ping pid와 Pong pid (Ping, Pong)

/* --------- */
/* Functions */
/* --------- */

/* 0. Main process */
pid_t* init_pingpong(int pairs, int iter, int num_cpus);

/* 1. Ping process */
void recv_pong();
void end_ping();

/* 2. Pong process */
void recv_ping();
void end_pong();

#endif