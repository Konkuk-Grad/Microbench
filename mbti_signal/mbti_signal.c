#include "mbti_signal.h"

/*  
 * Signal Test의 Main 함수로 다음과 같은 실행 방식을 가짐.
 *  1. sig_test_init()를 실행하여 test process들을 만들고 pid를 가짐
 *  2. sig_test_exec()를 실행하여 test process들에 SIGCONT를 전송하여 Test를 진행하고
 *     결과 값을 받음.
 *  3. 결과 값을 main 함수에 return
 * 
 *  Parameter
 *      - int topology: 특정 topology를 선택하는 값
 *      - int processes: process들 (혹은 process 쌍들)의 개수
 *      - int iter: pattern 반복 횟수
 *      - int num_cpus: process들이 실행되는 코어의 수
 * 
 *  Return value
 *      - double measure_time: 실험 측정 결과값. (시간값)
 */

double sig_test(int topology, int processes, int iter, int num_cpus){
    pid_t* trig_pid = NULL;
    double measure_time = 0;
#ifdef CASE18
    DEBUGMSG("[CASE18] topology: %d, Number of processes' pairs: %d, iter: %d, cores: %d\n",
            topology, processes, iter, num_cpus);
#endif
    trig_pid = sig_test_init(topology, processes, iter, num_cpus);

    if(trig_pid == NULL){
        // All kill?
        PRINTERROR("sig_test_init failed!\n");
        return -1;
    }

    // DEBUGMSG("processes: %d, trig_pid = %p\n", processes, trig_pid);
    // for(int i = 0; i < processes; i++){
    //     DEBUGMSG("trig_pid[%d]: %d\n", i, trig_pid[i]);
    // }
    
    measure_time = sig_test_exec(trig_pid, processes, topology);

    free(trig_pid);

    return measure_time;
}

// int sig_test_attr(); // Signal Test Setting Attribute

/* 
 * Topology에 해당하는 함수를 실행, 해당 함수는 process 들을
 * 사용자의 매개변수에 따라 실행해놓고 process들을 정지시켜놓아야 함. 
 */
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus){ // Signal Test Initialization
    pid_t* pid_list = NULL;
    switch(topology){
        case 1: // Ping-pong
            pid_list = init_pingpong(processes, iter, num_cpus);
            break;
        default:
            break;
    }

    if(pid_list == NULL){
        PRINTERROR("Normal Initializing failed\n");
    } 
#ifdef CASE19
    else{
        int i = 0;
        DEBUGMSG("[CASE19] pid_list: ");
        do{
            printf("%d ", pid_list[i]);
        }while(pid_list[++i] != 0);
        printf("\n");
    }
#endif

    return pid_list;
} 

/*
 * 1. Process들에게서 시간을 받아오기 위한 Message Queue 생성
 * 2. trig_pid 배열로 받아온 pid들에 SIGCONT를 보냄.
 * 3. 모두 완료되었을 시 시간 계산
 * 4. return_time으로 측정된 시간 반환
 */
double sig_test_exec(pid_t *trig_pid, int processes, int topology){ // Signal Test
    int status = 2;
    mqd_t mfd = -1;
    struct mq_attr msgqattr;
    time_msg msgbox;

    double* measure_times;
    measure_times = (double *)malloc(sizeof(double) * processes);
    // DEBUGMSG("MQ_FILE: %s, getpid(): %d\n", MQ_FILE, getpid());

    // Triggered by user
#ifdef USER_EXEC
    DEBUGMSG("Triggered by user\n");
    sleep(2);
#else
    DEBUGMSG("Wait until getting SIGCONT\n");
    kill(getpid(), SIGSTOP);
#endif

    // Make a message queue
    msgqattr.mq_flags = 0;
    msgqattr.mq_maxmsg = 1;
    msgqattr.mq_msgsize = sizeof(time_msg);
    msgqattr.mq_curmsgs = 0;

    if((mfd = mq_open(MQ_FILE, O_RDWR | O_CREAT, 0666, &msgqattr)) == -1){
        perror("open_error");
        exit(-1);
    }

    msgbox.mtype = 0;
    msgbox.measure_time = -1;

    // Send a dummy message to count completed processes 
    if(mq_send(mfd, (const char *)&msgbox, msgqattr.mq_msgsize, 1) == -1){
        perror("[main] send error");
        return -1;
    }

    // Start Tests (Send signals to child processes)
    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    // Wait until all processes are done
    // kill(getpid(), SIGTSTP);
    kill(getpid(), SIGSTOP);

    // Send signals to child processes to send messages
    for(int i = 0; i < processes; i++){
#ifdef CASE22
        clock_gettime(CLOCK_MONOTONIC, &cont_point);
        PRINTWARN("[CASE22] {pid: %d} Cont pid: %d / %ld ns\n", getpid(), trig_pid[i], (cont_point.tv_sec * 1000000000 + cont_point.tv_nsec));
#endif
        kill(trig_pid[i], SIGCONT);
    }

    // Receive messages from completed child processes
    for(int i = 0; i < processes; i++){
        if(mq_receive(mfd, (char *)&msgbox, msgqattr.mq_msgsize, NULL) == -1){
            perror("receive error");
        } else {
            measure_times[i] = msgbox.measure_time;
#ifdef CASE20
            if(!WIFEXITED(status)){
                DEBUGMSG("[CASE20] Recv {%f} ms \n", measure_times[i]);
            }
#endif
        }
    }

    mq_close(mfd);
    mq_unlink(MQ_FILE);

    // Calculate received times
    double return_time = 0;
    switch(topology){
        case 1: // Ping-pong
            for(int i = 0; i < processes; i++){
                return_time += measure_times[i];
            }
            return_time /= processes;
            break;

        default:
            break;
    }

    free(measure_times);
    return return_time;
} 