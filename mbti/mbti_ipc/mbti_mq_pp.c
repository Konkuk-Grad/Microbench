#include "mbti_mq_pp.h"

double mq_pingpong(int pairs, int iter, int cores) {

	pid_t pid, wpid;
	int status = 0;
	int mq_prio = 1;
	double value = 0;
	double measure_time = 0;
	struct mq_attr attr; // 메세지 큐 특성 설정 구조체
    	mqd_t mq_pp_mqdes; // 메세지 큐 디스크립터 타입

        attr.mq_maxmsg = MQ_PP_MAX_MSG; // 메세지 큐 크기
        attr.mq_msgsize = MQ_PP_MSG_SIZE; // 메세지 크기(바이트)
	mq_pp_mqdes = mq_open(MQ_PP_NAME, O_CREAT | O_RDWR, 0660, &attr);

	for(int i = 0; i < 2 * pairs; i++) {
        	pid = fork();
		if (pid == 0) { // child
			if(i % 2 == 0) { // ping
				measure_time = mq_ping(i, iter, cores);
        			if(mq_send(mq_pp_mqdes, (char *)&measure_time, MQ_PP_MSG_SIZE, mq_prio) == -1) {
            				perror("main_mq_send() fail\n");
				}
			}
			else {
				mq_pong(i, iter, cores);
			}
			exit(0);
		}
	}

	while ((wpid = wait(&status)) > 0); // waits for all the child processes

	for (int i = 0; i < pairs; i++) {
		if(mq_receive(mq_pp_mqdes, (char *)&value, MQ_PP_MSG_SIZE, &mq_prio) != -1) {
			//printf("get %f time from pair%d with priority %d\n", value, i, mq_prio);
			measure_time += value;
		}
	}

	mq_close(mq_pp_mqdes);
    	mq_unlink(MQ_PP_NAME);

	return measure_time / pairs;
}

double mq_ping(int num, int iter, int cores){

	struct mq_attr attr; // 메세지 큐 특성 설정 구조체
    	mqd_t ping_mqdes, pong_mqdes; // 메세지 큐 디스크립터 타입
    	unsigned int ping_prio = 10, pong_prio;
	double time = 0, measure_time = 0;
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
	
	set_core_affinity(cores); 

	for(int i = 0; i < iter; i++) {
        	//printf("%d ping sending a message %d with priority %d\n", num, i, ping_prio);
        	if(mq_send(pong_mqdes, (char *)&i, MQ_MSG_SIZE, ping_prio) == -1) {
            		perror("mq_send() fail\n");
		}
		
		if(mq_receive(ping_mqdes, (char *)&value, MQ_MSG_SIZE, &pong_prio) != -1) {
			//printf("%d ping received a message %d with priority %d\n", num, value, pong_prio);
		}

		if(i == 0) {
			get_time_diff(0); // 시간 측정 시작
		}
    	}
	time = get_time_diff(1);
    	//printf("Pair%d PingPong Elapsed Time: %f \n", num/2, time);
    	
	mq_close(ping_mqdes);
    	mq_unlink(strcat(ping_name, ping_num));

    	return time;
}

int mq_pong(int num, int iter, int cores){

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

	set_core_affinity(cores); 
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
	
	cpu_set_t cpu_mask;
    	size_t size;	
	pid_t pid = getpid();

	CPU_ZERO(&cpu_mask);
	for(int i = 0; i < cores; i++) {
        	CPU_SET(i, &cpu_mask);
    	}

	if(sched_setaffinity(pid, sizeof(cpu_mask), &cpu_mask)) {
		printf("failed to get affinity\n");
		exit(1);
	}
}

/* Measure time different */
double get_time_diff(unsigned int flag) { // flag 0:start, 1:end

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
	return (double)ret_diff/1000000; //나노세컨을 mili세컨으로 바꿔서 리턴
}

void shm_create() {

	if((mq_pp_shmid = shmget((key_t)SHM_KEY_NUM, SHM_MEM_SIZE, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        	printf("There was shared memory.");
        	mq_pp_shmid = shmget((key_t)SHM_KEY_NUM, SHM_MEM_SIZE, IPC_CREAT|0666);

        	if(mq_pp_shmid == -1) {
            		perror("Shared memory create fail");
            		exit(1);
        	}
        	else {
            		shm_free();
            		mq_pp_shmid = shmget((key_t)SHM_KEY_NUM, SHM_MEM_SIZE, IPC_CREAT| 0666);
            		if(mq_pp_shmid == -1) {	
                		perror("Shared memory create fail");
                		exit(1);
            		}
        	}
    }
}

void shm_write(long long *shareddata, int size) {
	
	void *shmaddr;
	if(size > SHM_MEM_SIZE) {
		printf("Shared memory size over");
        	exit(1);
    	}

    	if((shmaddr = shmat(mq_pp_shmid, (void *)0, 0)) == (void *)-1) {
        	perror("Shmat failed");
        	exit(1);
    	}
    
	memcpy((long long *)shmaddr, shareddata, size);

    	if(shmdt(shmaddr) == -1) {
        	perror("Shmdt failed");
        	exit(1);
	}
}

void shm_read(long long *sMemory) {
    
	void *shmaddr;

    	if((shmaddr = shmat(mq_pp_shmid, (void *)0, 0)) == (void *)-1) {
        	perror("Shmat failed");
		exit(1);
    	}

    	memcpy(sMemory, (char *)shmaddr, sizeof(long long));

    	if(shmdt(shmaddr) == -1) {
        	perror("Shmdt failed");
		exit(1);
    	}
}

void shm_free() {

    	if(shmctl(mq_pp_shmid, IPC_RMID, 0) == -1) {
        	perror("Shmctl failed");
		exit(1);
    	}
}
