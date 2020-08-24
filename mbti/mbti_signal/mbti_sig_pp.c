#include "mbti_sig_pp.h"

/* 1) Ping Process */
void recv_pong(){
    // clock_gettime(CLOCK_MONOTONIC, &end_point); // Individual checking time
    
    if(curr_iter_count++ < user_iter_count){
        DEBUGMSG("[I] I->O ping_count: %d\n", recv_ping_count); // Send ping
        kill(pong_pid, SIGUSR1);        
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end_point);
        DEBUGMSG("[Terminate][I] I->O ping_count: %d\n", --curr_iter_count);
        kill(pong_pid, SIGUSR2); // Terminate pong process
    }
}

void end_ping(){
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

        if(mq_send(mfd, (const char *)&msgbox, sizeof(time_msg), 0) == -1){
            perror("[end_ping] send time error");
            exit(-1);
        }
    }

    DEBUGMSG("[Total  ] {%f} ms\n", measure_time);
    DEBUGMSG("[Average] {%f} ms/iter \n", measure_time / user_iter_count);

    exit(0);
}

/* 2) Pong Process */
void recv_ping(){
    if(recv_ping_count++ < user_iter_count){
        kill(ping_pid, SIGUSR1); // SEND PONG
        DEBUGMSG("[O] O->I recv_ping_count: %d\n", recv_ping_count); // Send pong
    } else {
        // dummy else
    }
}

void end_pong(){
    DEBUGMSG("[endpong][O] recv_ping_count: %d\n", recv_ping_count);
    kill(ping_pid, SIGUSR2);
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
        } else {
            DEBUGMSG("pid: %d, fork: %d\n", getpid(), i);
        }
    }

    return pid_arr;
}