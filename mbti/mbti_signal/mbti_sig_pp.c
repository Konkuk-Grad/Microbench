#include "mbti_sig_pp.h"

/* 1) Ping Process */
void recv_pong(){
    // Receive pong
    if(++curr_iter_count < user_iter_count){
#ifdef CASE23
        PRINTLOG("[CASE23] {pid: %d} Receive pong, Send ping curr_iter_count: %d, user_iter_count: %d\n", getpid(), curr_iter_count, user_iter_count);
#endif
        kill(pong_pid, SIGUSR1); // Send ping
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end_point);
#ifdef CASE24
        PRINTWARN("[CASE24] {pid: %d} Receive pong, Terminate curr_iter_count: %d, user_iter_count: %d\n", getpid(), curr_iter_count, user_iter_count);
#endif
        kill(pong_pid, SIGUSR2); // Terminate pong process
    }
}

void end_ping(){
#ifdef CASE27
    PRINTWARN("{pid: %d} Receive SIGUSR2!\n", getpid());
#endif
    time_msg msgbox;

    int mfd = -1, tmp = -1;
    if((mfd = mq_open(MQ_FILE, O_RDWR)) == -1){
        perror("[end_ping] open_error");
        exit(-1);
    }

    if(mq_receive(mfd, (char *)&msgbox, sizeof(time_msg), NULL) == -1){
        perror("[end_ping] recv error");
        exit(-1);
    } else {
        if(msgbox.measure_time == -complete_processes){
            // Test ends then call parent
            kill(getppid(), SIGCONT);
        } else {
            msgbox.measure_time--;
            if(mq_send(mfd, (const char *)&msgbox, sizeof(time_msg), 0) == -1){
                perror("[end_ping] negative send error");
                exit(-1);
            }
        }

        kill(getpid(), SIGSTOP);

        msgbox.mtype = 0;
        msgbox.measure_time = (end_point.tv_sec - start_point.tv_sec) * 1000 + (double)(end_point.tv_nsec - start_point.tv_nsec) / 1000000;

        if((tmp = mq_send(mfd, (const char *)&msgbox, sizeof(time_msg), 0)) == -1){
            perror("[end_ping] send time error");
            exit(-1);
        } else {
#ifdef CASE25
            DEBUGMSG("[CASE25] {pid: %d} mq_send ret: %d, Send {%f} ms \n", getpid(), tmp, msgbox.measure_time);
#endif
        }
    }

    exit(getpid());
}

/* 2) Pong Process */
void recv_ping(){
    if(recv_ping_count++ < user_iter_count){
#ifdef CASE26
        PRINTLOG("[CASE26] {pid: %d} Receive ping, Send pong recv_ping_count: %d, user_iter_count: %d\n", getpid(), recv_ping_count, user_iter_count);
#endif
        kill(ping_pid, SIGUSR1); // SEND PONG
    } else {
        // dummy else
    }
}

void end_pong(){
    int ret = -2;
    ret = kill(ping_pid, SIGUSR2);
#ifdef CASE27
    PRINTWARN("[CASE27] {pid: %d} kill ret: %d\n", getpid(), ret);
#endif
    exit(0);    
}

pid_t* init_pingpong(int pairs, int iter, int num_cpus){

    sigset_t sigset; // Block SIGUSR1,2
    sigset_t oldset; // Prev sigset
    struct sigaction act[2]; // 0: SIGUSR1, 1: SIGUSR2
    cpu_set_t *cpu_mask;
    size_t size;
    int result;

    user_iter_count = iter;
    complete_processes = pairs;
    pid_arr = (pid_t *)malloc(sizeof(pid_t) * pairs);

    /* Set a number of iterations */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);

    /* Set cpu affinities */
    cpu_mask = CPU_ALLOC(num_cpus);
    if(cpu_mask == NULL){
        perror("CPU_ALLOC");
        exit(-1);
    }

    size = CPU_ALLOC_SIZE(num_cpus);
    CPU_ZERO_S(size, cpu_mask);

    for(int i = 0; i < num_cpus; i++){
        CPU_SET_S(i, size, cpu_mask);
    }
    
    for(int i = 0; i < pairs; i++){
        if(!(pid_arr[i] = fork())){ // Child (Ping)
            if(pong_pid = fork()){ // Parent (Ping)

                /* Setting Handler */            
                act[0].sa_handler = recv_pong;
                act[0].sa_flags = 0;

                act[1].sa_handler = end_ping;
                act[1].sa_flags = 0;

                sigaction(SIGUSR1, &act[0], NULL); // Received signal
                sigaction(SIGUSR2, &act[1], NULL); // End signal

                sigprocmask(SIG_BLOCK, &sigset, &oldset);

                /* Set cpu affinity */
                result = sched_setaffinity(0, sizeof(*cpu_mask), cpu_mask);
                if(result == -1){
                    printf("pid [%d] sched_setaffinity failed!\n", getpid());
                }            

                /* Ping-pong logic */
#ifdef CASE22
                clock_gettime(CLOCK_MONOTONIC, &wait_point);
                PRINTWARN("[CASE22] {pid: %d} %ld ns\n", getpid(), (wait_point.tv_sec * 1000000000 + wait_point.tv_nsec));
#endif
                kill(getpid(), SIGSTOP);
                clock_gettime(CLOCK_MONOTONIC, &start_point);
                recv_pong();
                while(1){
                    sigsuspend(&oldset);
                }
            } else { // Child (Pong)
                ping_pid = getppid();

                /* Setting Handler */
                act[0].sa_handler = recv_ping;
                act[0].sa_flags = 0;

                act[1].sa_handler = end_pong;
                act[1].sa_flags = 0;

                sigaction(SIGUSR1, &act[0], NULL);
                sigaction(SIGUSR2, &act[1], NULL);

                sigprocmask(SIG_BLOCK, &sigset, &oldset);

                /* Set cpu affinity */
                result = sched_setaffinity(0, sizeof(*cpu_mask), cpu_mask);
                if(result == -1){
                    printf("pid [%d] sched_setaffinity failed!\n", getpid());
                }         

                while(1){
                    sigsuspend(&oldset);
                }
            }
        } else if(pid_arr[i] < 0) { // Fork Error
            PRINTERROR("[CASE19] Fork Failed!\n");
            for(int j = 0; j < i; j++){
                kill(pid_arr[i], SIGKILL);
            }
            free(pid_arr);
            return NULL;
        } else { // Parent (Fork Success)
#ifdef CASE21
            DEBUGMSG("[CASE21] pid: %d, fork: %d\n", getpid(), i);
#endif
        }
    }

#ifdef CASE21
    PRINTRESULT("[CASE21] Return children pid_list\n");
    DEBUGMSG("[CASE21] ");
    for(int i = 0; i < pairs; i++){
        printf("%d ", pid_arr[i]);
    }
    printf("\n\n");
#endif

    return pid_arr;
}