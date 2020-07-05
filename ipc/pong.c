#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>

#define COUNT 10
#define MSG_SIZE 4
#define PING_NAME "/mqueue_ping"
#define PONG_NAME "/mqueue_pong"
#define MAX_PRIO 32

long long GetTimeDiff(unsigned int nFlag);

int main(){

	struct mq_attr attr; // 메세지 큐 특성 설정 구조체
	int i, value;
	unsigned int ping_prio, pong_prio = 20;
	mqd_t ping_mqdes, pong_mqdes; // 메세지 큐 디스크립터 타입

	attr.mq_maxmsg = 10; // 메세지 큐 크기
	attr.mq_msgsize = MSG_SIZE; // 메세지 크기(바이트)

	ping_mqdes = mq_open(PING_NAME, O_CREAT | O_RDWR, 0660, &attr);
	pong_mqdes = mq_open(PONG_NAME, O_CREAT | O_RDWR, 0660, &attr);

	if (ping_mqdes < 0 || pong_mqdes < 0) {
		perror("mq_open() fail\n");
		exit(0);
	}

	for(i = 0; i < COUNT; i++) {

		if(i == 0) {
			GetTimeDiff(0);
		}
		if(mq_receive(pong_mqdes, (char *)&value, MSG_SIZE, &ping_prio) != -1) {
			//printf("received a message %d with priority %d\n", value, ping_prio);
		}

		//printf("sending a message %d with priority %d\n", i, pong_prio);
		if(mq_send(ping_mqdes, (char *)&i, MSG_SIZE, pong_prio) == -1) {
			perror("mq_send() fail\n");
		}
		pong_prio++;

	}
	printf("Elapsed Time: %lld \n", GetTimeDiff(1)/10);

	mq_close(pong_mqdes);
	mq_unlink(PONG_NAME);
}

long long GetTimeDiff(unsigned int nFlag) {
	const long long NANOS = 1000000000LL;
	static struct timespec startTS, endTS;
	static long long retDiff = 0;

	if(nFlag == 0) {
		retDiff = 0;
		if(clock_gettime(CLOCK_MONOTONIC, &startTS) == -1) {
			printf("failed to call clock_gettime\n");
		}
	}
	else {
		if(clock_gettime(CLOCK_MONOTONIC, &endTS) == -1) {
			printf("failed to call clock_gettime\n");
		}
		retDiff = NANOS * (endTS.tv_sec - startTS.tv_sec) + (endTS.tv_nsec - startTS.tv_nsec);
	}

	return retDiff/1000; //나노세컨을 마이크로세컨으로 바꿔서 리턴
}
