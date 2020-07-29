#include "../mbt_headers/mbt_signal.h"
int add(int a, int b){
    return a + b;
}
int sig_test(int topology, int processes, int iter, int num_cpus){
    pid_t* trig_pid = NULL;
    trig_pid = sig_test_init(topology, processes, iter, num_cpus);

    printf("processes: %d, trig_pid = %p\n", processes, trig_pid);

    for(int i = 0; i < processes; i++){
        printf("trig_pid[%d]: %d\n", i, trig_pid[i]);
    }
    sig_test_exec(trig_pid, processes, topology);

    free(trig_pid);

}
// int sig_test_attr(); // Signal Test Setting Attribute
pid_t* sig_test_init(int topology, int processes, int iter, int num_cpus){ // Signal Test Initialization
    switch(topology){
        case 1: // Ping-pong
            printf("hello i'm ping-pong\n");
            pid_t* tmp = NULL;
            printf("[b] tmp: %p\n", tmp);
            tmp = init_pingpong(processes, iter, num_cpus);
            printf("[a] tmp: %p\n", tmp);
            return tmp;
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

    printf("MQ_FILE: %s, getpid(): %d\n", MQ_FILE, getpid());

    sleep(5);
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

    kill(getpid(), SIGSTOP);

    for(int i = 0; i < processes; i++){
        kill(trig_pid[i], SIGCONT);
    }

    for(int i = 0; i < processes; i++){
        if(mq_receive(mfd, (char *)&msgbox, msgqattr.mq_msgsize, NULL) == -1){
            perror("receive error");
        } else {
            measure_times[i] = msgbox.measure_time;
            printf("measure_times[%d]: %f\n", i, measure_times[i]);
        }
    }

    double sum = 0;
    switch(topology){
        case 1: // Ping-pong
            
            for(int i = 0; i < processes; i++){
                sum += measure_times[i];
            }

            free(measure_times);
            return sum / processes;
        default:
            break;
    }

    return 0;
} 