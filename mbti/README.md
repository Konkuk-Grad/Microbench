# MicroBenchmark Test Interface (MBTI)

## 0. 개발자

---

- 김상재: <hestarium@konkuk.ac.kr>
- 박성준: <parks8109@naver.com>
- 이종빈: <empty_paper@icloud.com>
- 장서연: <4066898@naver.com>

## 1. 폴더 구조

---

각 폴더 개발자들은 폴더를 생성하여 개발을 진행하도록 한다.
![Folder_Structure](.readme/folder_structure.jpg)

## 2. 소스파일 구조

---

![Source_Code_Structure](.readme/slide1.jpg)

## 3. 헤더파일 구조

---

```h
/***** Parent Header *****/
#include "../mbti.h" 

// 현재 Header 파일의 시작을 나타낸다.

/* 중요 */
// 각 측정 분야의 최상위 Header는 Parent Header(../mbti.h)와의
// 중복 선언 방지를 위하여 아래와 같은 처리를 반드시 하여야 함.

// #ifndef 와 #endif 사이에 정의되는 모든 것들은 현재 및 하위 헤더만 사용가능하며
// Parent Header에서는 사용이 불가능하다.

/* 권장 사항 */
// 전처리문자 규칙은 헤더 파일을 아래와 같은 양식으로 적는 것이다.
// 헤더파일 이름으로 적으며, 맨 앞뒤에 밑줄 두개를 추가한다.
// ex. ab_def_123.h => __AB_DEF_123_H__

// Start mbti_xx.h
#ifndef __MBT_XX_H__
#define __MBT_XX_H__

/* ------- */
/* Defines */
/* ------- */
#define MQ_FILE "/mbti_sig_mq"

/* ------- */
/* Headers */
/* ------- */
#include <signal.h>

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
struct timespec start_point, end_point;

// 여기에 정의되는 함수들은 반드시 헤더파일과 동일한 이름을 갖는 .c 파일에 구현한다.
// ex. sig_test는 mbti_xx.h에 선언되어 있으므로 mbti_xx.c 파일에 구현한다.
/* --------- */
/* Functions */
/* --------- */
double sig_test(int topology, int processes, int iter, int num_cpus);

/* -------- */
/* Topology */
/* -------- */
#include "mbti_sig_pp.h"

// End mbti_xx.h
#endif
```

## 4. 소스 코드 구조

---

```c
// mbti_xx.c
// mbti_xx.h에서 선언된 함수들이 작성되어있는 코드이다.
#include "mbti_xx.h"

double sig_test(int topology, int processes, int iter, int num_cpus){
    ...
}

```

## 5. 컴파일 명령

---

- 실행 파일명은 **"mbti.out"** 으로 하고 mbti_main.c를 포함한 **모든 폴더의 c파일**을 포함시켜야 한다.
- 컴파일 옵션은 현재는 **-lrt** 를 붙여야 하며 이 이후에 추가될 예정이다.

- **cmake** 를 이용하여 컴파일의 용이성을 찾으려 하고 있다.

```bash
gcc -o mbti.out mbti_main.c mbti_signal/mbti_signal.c mbti_signal/mbti_sig_xx.c -lrt 
```