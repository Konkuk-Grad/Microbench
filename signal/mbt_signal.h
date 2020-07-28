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

// #include "mbt_sig_pp.h"

/* --------- */
/*  Define   */
/* --------- */

/* ----------- */
/*  Structure  */
/* ----------- */

typedef struct __time_msg{
    int mtype;
    double measure_time;
}time_msg;


/* ------------------ */
/*  Global Variables  */
/* ------------------ */
struct timespec start_point, end_point;
double measure_time;
// pid_t ping_pid, pong_pid;

// int curr_iter_count = 0;
// int recv_pong_count = 0;

int user_iter_count = 0;

/* --------- */
/* Functions */
/* --------- */

// Signal Testing Function
int sig_test(); // Signal Test Main
int sig_test_attr(); // Signal Test Setting Attribute
int sig_test_init(); // Signal Test Initialization
int sig_test_exec(); // Signal Test

/*** Topology ***/

/** 1. Ping-pong **/
void init_pingpong();

/* 1) Ping Process */
void recv_pong();
void end_ping();

/* Pong Process */
void recv_ping();
void end_pong();

#include "mbt_sig_pp.h"
