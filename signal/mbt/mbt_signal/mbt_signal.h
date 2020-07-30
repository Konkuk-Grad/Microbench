/***** Parent Header *****/
#include "../mbt.h"

// Start mbt_signal.h
#ifndef __MBT_SIG_H__
#define __MBT_SIG_H__

/* ------- */
/* Defines */
/* ------- */
#define MQ_FILE "/mbt_sig_mq"

/* ------- */
/* Headers */
/* ------- */
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
struct timespec start_point, end_point; // 측정 시작 시각, 측정 종료 시각
double measure_time; // 측정 시간
int complete_processes; // 테스트 완료된 프로세스 개수
int user_iter_count; // 사용자가 입력한 패턴 반복 횟수

/* --------- */
/* Functions */
/* --------- */
double sig_test(int topology, int processes, int iter, int num_cpus);
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus);
double sig_test_exec(pid_t *trig_pid, int processes, int topology);

/* -------- */
/* Topology */
/* -------- */

/* 1. Ping-pong */
#include "mbt_sig_pp.h"

#endif