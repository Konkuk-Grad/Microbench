#include "mbti_mq_pp.h"

int mq_pingpong(int pairs, int iter, int cores) {

	pid_t pid;

	set_core_affinity(cores);

	for(int i = 0; i < 2 * pairs; i++) {
        	pid = fork();
		if (pid == 0) { // child
			if(i % 2 == 0) { // ping
				mq_ping(i, iter);
			}
			else {
				mq_pong(i, iter);
			}
			exit(0);
		}
	}
	return 0;
}

int mq_ping(int num, int iter){

	struct mq_attr attr; // 메세지 큐 특성 설정 구조체
    	mqd_t ping_mqdes, pong_mqdes; // 메세지 큐 디스크립터 타입
    	unsigned int ping_prio = 10, pong_prio;
	int value;
	char ping_name[30] = "/mq_ping";
	char pong_name[30] = "/mq_pong";
	char ping_num[15] = { 0 };
	char pong_num[15] = { 0 };
	
	sprintf(ping_num, "%d", num);	
	sprintf(pong_num, "%d", num + 1);	
    	
	attr.mq_maxmsg = MQ_MAX_MSG; // 메세지 큐 크기
    	attr.mq_msgsize = MQ_MSG_SIZE; // 메세지 크기(바이트)

	ping_mqdes = mq_open(strcat(ping_name, ping_num), O_CREAT | O_RDWR, 0660, &attr);
    	pong_mqdes = mq_open(strcat(pong_name, pong_num), O_CREAT | O_RDWR, 0660, &attr);

    	if (ping_mqdes < 0 || pong_mqdes < 0) {
        	perror("mq_open() fail\n");
        	exit(0);
    	}
	
	get_time_diff(0); // 시간 측정 시작

	for(int i = 0; i < iter; i++) {
        	//printf("%d ping sending a message %d with priority %d\n", num, i, ping_prio);
        	if(mq_send(pong_mqdes, (char *)&i, MQ_MSG_SIZE, ping_prio) == -1) {
            		perror("mq_send() fail\n");
		}
		if(mq_receive(ping_mqdes, (char *)&value, MQ_MSG_SIZE, &pong_prio) != -1) {
			//printf("%d ping received a message %d with priority %d\n", num, value, pong_prio);
		}
    	}
    	printf("Pair%d One PingPong Average Elapsed Time: %lld \n", num/2, get_time_diff(1)/(long long)iter);

    	mq_close(ping_mqdes);
    	mq_unlink(strcat(ping_name, ping_num));

    	return 0;
}

int mq_pong(int num, int iter){

        struct mq_attr attr; // 메세지 큐 특성 설정 구조체
        mqd_t ping_mqdes, pong_mqdes; // 메세지 큐 디스크립터 타입
        unsigned int ping_prio, pong_prio = 20;
	int value;
	char ping_name[30] = "/mq_ping";
	char pong_name[30] = "/mq_pong";
	char ping_num[15] = { 0 };
	char pong_num[15] = { 0 };
	
	sprintf(ping_num, "%d", num - 1);	
	sprintf(pong_num, "%d", num);	

        attr.mq_maxmsg = MQ_MAX_MSG; // 메세지 큐 크기
        attr.mq_msgsize = MQ_MSG_SIZE; // 메세지 크기(바이트)

        ping_mqdes = mq_open(strcat(ping_name, ping_num), O_CREAT | O_RDWR, 0660, &attr);
        pong_mqdes = mq_open(strcat(pong_name, pong_num), O_CREAT | O_RDWR, 0660, &attr);

        if (ping_mqdes < 0 || pong_mqdes < 0) {
                perror("mq_open() fail\n");
                exit(0);
        }

        for(int i = 0; i < iter; i++) {
                if(mq_receive(pong_mqdes, (char *)&value, MQ_MSG_SIZE, &ping_prio) != -1) {
                        //printf("%d pong received a message %d with priority %d\n", num, value, ping_prio);
                }
                //printf("%d pong sending a message %d with priority %d\n", num, i, pong_prio);
                if(mq_send(ping_mqdes, (char *)&i, MQ_MSG_SIZE, pong_prio) == -1) {
                        perror("mq_send() fail\n");
                }
        }

        mq_close(pong_mqdes);
        mq_unlink(strcat(pong_name, pong_num));

        return 0;
}

/* Set cpu affinities */
void set_core_affinity(int cores) {
	
	cpu_set_t *cpu_mask;
    	size_t size;	

    	cpu_mask = CPU_ALLOC(cores);
    	if(cpu_mask == NULL) {
        	perror("CPU_ALLOC ERROR");
        	exit(-1);
    	}
	size = CPU_ALLOC_SIZE(cores);
	CPU_ZERO_S(size, cpu_mask);
	for(int i = 0; i < cores; i++) {
        	CPU_SET_S(i, size, cpu_mask);
    	}
}

/* Measure time different */
long long get_time_diff(unsigned int flag) { // flag 0:start, 1:end

	const long long NANOS = 1000000000LL;
	static struct timespec start_ts, end_ts;
	static long long ret_diff = 0;

	if(flag == 0) {
		ret_diff = 0;
		if(clock_gettime(CLOCK_MONOTONIC, &start_ts) == -1) {
			printf("failed to call clock_gettime\n");
		}
	}
	else {
		if(clock_gettime(CLOCK_MONOTONIC, &end_ts) == -1) {
			printf("failed to call clock_gettime\n");
		}
		ret_diff = NANOS * (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec);
	}
	return ret_diff/1000; //나노세컨을 micro세컨으로 바꿔서 리턴
}
