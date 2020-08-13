#define _GNU_SOURCE

// Micro Benchmark Test Interface (MBTI) // 
/***** Parent Headers *****/
#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#endif

// Start mbti.h
#ifndef __MBT_H__
#define __MBT_H__

/* ------- */
/* Headers */
/* ------- */

/* 1. Signal Header */
#include "mbti_signal/mbti_signal.h"

/* 2. IPC Header */
// #include "mbti_ipc/mbti_ipc.h"

/* 3. Semaphore Header */
#include "mbti_semaphore/mbti_sem.h"

/* 4. Mutex Header */
// #include "mbti_mutex/mbti_mutex.h"

/* 5. TCP/IP Header */
// #include "mbti_network/mbti_network.h"

#endif