#define MQ_FILE "/mbt_mq"

int sig_test(int topology, int processes, int iter, int num_cpus){
    pid_t *trig_pid = NULL;
    trig_pid = sig_test_init();
    sig_test_exec(trig_pid, processes);

    free(trig_pid);

}
int sig_test_attr(); // Signal Test Setting Attribute
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus){ // Signal Test Initialization
    switch(topology){
        case 1: // Ping-pong
            return init_pingpong(processes, iter, num_cpus);
        default:
            return NULL;
    }
} 
double sig_test_exec(pid_t *trig_pid, int processes){ // Signal Test
    int mfd = -1, tmp = -1;
    time_msg recv_time;
    double* measure_times;
    measure_times = (double *)malloc(sizeof(double) * processes);
    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    kill(getpid(), SIGSTOP);

    if((mfd = mq_open(MQ_FILE, O_RDONLY | O_CREAT, 0666, &attr)) == -1){
        perror("open_error");
        exit(-1);
    }

    for(int i = 0; i < processes; i++){
        if((tmp = mq_receive(mfd, (char *)&recv_time, attr.mq_msgsize, NULL)) == -1){
            perror("receive error");
        } else {
            measure_times[i] = recv_time.measure_time;
        }
    }

    switch(topology){
        case 1: // Ping-pong
            double sum = 0;
            for(int i = 0; i < processes; i++){
                sum += measure_times[i];
            }

            free(measure_times);
            return sum / processes;
    }
} 