#define _GNU_SOURCE

/* --------- */
/*  Headers  */
/* --------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>
#include <sched.h>
#include <mqueue.h>
#include <sys/types.h>

/* ------------------ */
/*  Global Variables  */
/* ------------------ */
pid_t *pid_arr;
pid_t ping_pid, pong_pid;

int curr_iter_count = 0;
int recv_ping_count = 0;

/* --------- */
/* Functions */
/* --------- */

/*** Topology ***/

/** 1. Ping-pong **/
pid_t* init_pingpong(int pairs, int iter, int num_cpus);

/* 1) Ping Process */
void recv_pong();
void end_ping();

/* Pong Process */
void recv_ping();
void end_pong();