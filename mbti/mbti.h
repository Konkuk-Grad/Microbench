#define _GNU_SOURCE

// Micro Benchmark Test Interface (MBTI) // 
/***** Parent Headers *****/
#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/**** Defines ****/
/** Print type **/
// Colors
#define ANSI_COLOR_RED "\x1b[31m" 
#define ANSI_COLOR_GREEN "\x1b[32m" 
#define ANSI_COLOR_YELLOW "\x1b[33m" 
#define ANSI_COLOR_BLUE "\x1b[34m" 
#define ANSI_COLOR_MAGENTA "\x1b[35m" 
#define ANSI_COLOR_CYAN "\x1b[36m" 
#define ANSI_COLOR_RESET "\x1b[0m" 

#ifdef __DEBUGMSG
#define DEBUGMSG(fmt, args...) fprintf(stderr, ANSI_COLOR_YELLOW"[DEBUGMSG]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)
#define PRINTWARN(fmt, args...) fprintf(stdout, ANSI_COLOR_YELLOW"[WARN]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)
#define PRINTLOG(fmt, args...) fprintf(stdout, ANSI_COLOR_BLUE"[LOG]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)
#else
#define DEBUGMSG(fmt, args...)
#define PRINTWARN(fmt, args...)
#define PRINTLOG(fmt, args...)
#endif

#define PRINTERROR(fmt, args...) fprintf(stderr, ANSI_COLOR_RED"[ERROR]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)

#define PRINTRESULT(fmt, args...) fprintf(stdout, ANSI_COLOR_GREEN"[RESULT]"ANSI_COLOR_CYAN"{%s:%d:%s()}: "ANSI_COLOR_RESET fmt, \
__FILE__, __LINE__, __func__, ##args)

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
#include "mbti_ipc/mbti_ipc.h"

/* 3. Semaphore Header */
#include "mbti_semaphore/mbti_sem.h"

/* 4. pthread Header */
#include "mbti_pthread/mbti_pthread.h"

/* 5. TCP/IP Header */
// #include "mbti_network/mbti_network.h"

#endif
