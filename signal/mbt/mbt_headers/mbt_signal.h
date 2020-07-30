/***** Parent Headers *****/
#include "mbt.h"

// Start mbt_signal.h
#ifndef __MBT_SIG_H__
#define __MBT_SIG_H__

#define MQ_FILE "/mbt_sig_mq"

#include <signal.h>
#include <mqueue.h>
#include <time.h>

/* ---------- */
/* Structures */
/* ---------- */
typedef struct __time_msg{
    long mtype;
    double measure_time;
}time_msg;

/* ---------------- */
/* Global Variables */
/* ---------------- */
int sum;

struct timespec start_point, end_point;
double measure_time;
int complete_processes;
int user_iter_count;


/* --------- */
/* Functions */
/* --------- */
double sig_test(int topology, int processes, int iter, int num_cpus);
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus);
double sig_test_exec(pid_t *trig_pid, int processes, int topology);


/************/
/* Topology */
/************/

/***** 1. Ping-pong *****/
#include "mbt_sig_pp.h"

#endif