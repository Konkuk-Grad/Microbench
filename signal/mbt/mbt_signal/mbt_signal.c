#include "../mbt_headers/mbt_signal.h"

double sig_test(int topology, int processes, int iter, int num_cpus){
    pid_t* trig_pid = NULL;
    double measure_time = 0;

    trig_pid = sig_test_init(topology, processes, iter, num_cpus);
#ifdef DEBUGMSG
    printf("processes: %d, trig_pid = %p\n", processes, trig_pid);
    for(int i = 0; i < processes; i++){
        printf("trig_pid[%d]: %d\n", i, trig_pid[i]);
    }
#endif
    
    measure_time = sig_test_exec(trig_pid, processes, topology);

    free(trig_pid);

    return measure_time;

}
// int sig_test_attr(); // Signal Test Setting Attribute
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus){ // Signal Test Initialization
    switch(topology){
        case 1: // Ping-pong
            return init_pingpong(processes, iter, num_cpus);
        default:
            return NULL;
    }

    return NULL;
} 

double sig_test_exec(pid_t *trig_pid, int processes, int topology){ // Signal Test
    mqd_t mfd = -1;
    struct mq_attr msgqattr;
    int tmp = -1;
    time_msg msgbox;

    double* measure_times;
    measure_times = (double *)malloc(sizeof(double) * processes);
#ifdef DEBUGMSG
    printf("MQ_FILE: %s, getpid(): %d\n", MQ_FILE, getpid());
#endif

    // Triggered by user
    sleep(5);

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

    if(mq_send(mfd, (const char *)&msgbox, msgqattr.mq_msgsize, 1) == -1){
        perror("[main] send error");
        return -1;
    }

    // Start Tests
    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    // Wait until all processes are done
    kill(getpid(), SIGTSTP);

    // Receive messages from test processes
    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    for(int i = 0; i < processes; i++){
        if(mq_receive(mfd, (char *)&msgbox, msgqattr.mq_msgsize, NULL) == -1){
            perror("receive error");
        } else {
            measure_times[i] = msgbox.measure_time;
#ifdef DEBUGMSG
            printf("measure_times[%d]: %f\n", i, measure_times[i]);
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