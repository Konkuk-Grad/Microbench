/***** Parent Header *****/
#include "../mbti.h"

// Start mbti_signal.h
#ifndef __MBT_SIG_H__
#define __MBT_SIG_H__

/* ------- */
/* Defines */
/* ------- */
#define MQ_FILE "/mbti_sig_mq"

#define ANSI_COLOR_RED "\x1b[31m" 
#define ANSI_COLOR_GREEN "\x1b[32m" 
#define ANSI_COLOR_YELLOW "\x1b[33m" 
#define ANSI_COLOR_BLUE "\x1b[34m" 
#define ANSI_COLOR_MAGENTA "\x1b[35m" 
#define ANSI_COLOR_CYAN "\x1b[36m" 
#define ANSI_COLOR_RESET "\x1b[0m" 

#ifdef __DEBUGMSG
#define DEBUGMSG(fmt, args...) fprintf(stderr, ANSI_COLOR_RED"[DEBUGMSG]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)
#endif

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
#include "mbti_sig_pp.h"

#endif