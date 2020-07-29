/***** Parent Headers *****/
#include "mbt_signal.h"

// Start mbt_sig_pp.h
#ifndef __MBT_SIG_PP_H__
#define __MBT_SIG_PP_H__

/***** Headers *****/
#include <sched.h>

/* ---------------- */
/* Global Variables */
/* ---------------- */
int curr_iter_count;
int recv_ping_count;

pid_t *pid_arr;
pid_t ping_pid, pong_pid;

/* --------- */
/* Functions */
/* --------- */
int sub(int a, int b);
int test2(int a, int b);

/* 0. Main process */
pid_t* init_pingpong(int pairs, int iter, int num_cpus);

/* 1. Ping process */
void recv_pong();
void end_ping();

/* 2. Pong process */
void recv_ping();
void end_pong();
#endif