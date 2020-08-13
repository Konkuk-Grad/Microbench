/***** Parent Header *****/

#include "../mbti.h"

// Start mbti_ipc.h
#ifndef __MBT_IPC_H__
#define __MBT_IPC_H__

/* ------- */
/* Defines */
/* ------- */

/* ------- */
/* Headers */
/* ------- */

#include <fcntl.h>
#include <mqueue.h>
#include <time.h>

/* ---------- */
/* Structures */
/* ---------- */

/* ---------------- */
/* Global Variables */
/* ---------------- */

/* --------- */
/* Functions */
/* --------- */

double ipc_test(int topology, int processes, int iter, int cores);

/* -------- */
/* Topology */
/* -------- */

/* 1. Ping-pong */
#include "mbti_mq_pp.h"

#endif
