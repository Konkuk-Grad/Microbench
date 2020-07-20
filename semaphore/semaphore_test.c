#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>

#define MAX 1

static sem_t Empty, Full, Mutex;
int buffer[MAX], front= 0, rear =0, cnt=0;
int N;
struct timespec begin, end;

int produce_item() {
	int item = rand()%50;
	printf("P made %d\n", item);
	return item;
}

int put_item(int item) {
	rear = (rear+1)%MAX;
	buffer[rear] = item;
	printf("P put %d\n", item);
}

int consume_item() {
	int item;
	item = buffer[front];
	front = (front + 1) % MAX;
	printf("C get %d\n", item);
	return item;
}

void* producer(void* arg) {
	int item;
	clock_gettime(CLOCK_MONOTONIC,&begin);
	printf("begin time : %ldns\n",begin.tv_nsec);
	while (cnt<N) {
		printf("cnt------>%d\n",cnt);
		item = produce_item();
		sem_wait(&Empty); 
		sem_wait(&Mutex);
		put_item(item);
		sem_post(&Mutex);
		sem_post(&Full);
	}
	printf("P is over\n");
	clock_gettime(CLOCK_MONOTONIC,&end);
	printf("end time : %ldns\n",end.tv_nsec);
	return 0;
}

void* consumer(void* arg) {
	int item;
	while(cnt<N) {
		sem_wait(&Full);
		sem_wait(&Mutex);
		item = consume_item();
		sem_post(&Mutex);
		sem_post(&Empty);
		cnt++;
	}
	printf("C is over\n");
	return 0;
}

int main(int argc, char *argv[]) {
	pthread_t thread1;
	pthread_t thread2;
	double time;

	N = atoi(argv[1]);
	if(isdigit(N)){
		printf("Input NUM Error! %d\n",N);
		return -1;
	}

	if((sem_init(&Empty,0,MAX))!=0)
	{
		printf("sem_init_Empty Error\n");
		return -1;
	}
	if((sem_init(&Mutex,0,1))!=0)
	{
		printf("sem_init_Mutex Error\n");
		return -1;
	}
	if((sem_init(&Full,0,0))!=0)
	{
		printf("sem_init_Full Error\n");
		return -1;
	}

	pthread_create(&thread1, NULL, consumer, NULL);
	pthread_create(&thread2, NULL, producer, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	sem_destroy(&Empty);
	sem_destroy(&Full);
	sem_destroy(&Mutex);
	time = (end.tv_sec-begin.tv_sec) * 1000 + (double)(end.tv_nsec-begin.tv_nsec) / 1000000;

	printf("time : %f\n", time/N);
	return 0;
}