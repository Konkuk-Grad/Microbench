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
int N = 1000;
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
/*
int setSemaphore(int semID, int semOP) {
	struct sembuf buf;

	buf.sem_num = 0;
	buf.sem_op = semOP;
	buf.sem_flg = SEM_UNDO;

	if (-1 == semop(semID, &buf, 1))
		printf("%d faild\n", semID);
}

void wait(semaphore s) {
	setSemaphore(s, -1);
}

void signal(semaphore s) {
	setSemaphore(s, 1);
}
*/

void* producer(void* arg) {
	int item;
	clock_gettime(CLOCK_MONOTONIC,&begin);
	printf("begin time : %ldns\n",begin.tv_nsec);
	while (1) {
		if(cnt<N){
			printf("cnt------>%d\n",cnt);
			item = produce_item();
			sem_wait(&Empty); 
			sem_wait(&Mutex);
			put_item(item);
			sem_post(&Mutex);
			sem_post(&Full);
		}else{
			printf("P is over\n");
			break;
		}

	}
	clock_gettime(CLOCK_MONOTONIC,&end);
	printf("end time : %ldns\n",end.tv_nsec);
	return 0;
}

void* consumer(void* arg) {
	int item;
	while (1) {
		if(cnt<N){
			sem_wait(&Full);
			sem_wait(&Mutex);
			item = consume_item();
			sem_post(&Mutex);
			sem_post(&Empty);
			cnt++;
		}
		else{
			printf("C is over\n");
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	pthread_t thread1;
	pthread_t thread2;
	double time;
	const long long NANOS = 1000000000LL;
	/*union semun {
		int	val;
		struct semid_ds* buf;
		unsigned short int* array;
	}arg;
	*/
	N = atoi(argv[1]);
	if(isdigit(N)!=0){
		printf("Error!\n");
		return 0;
	}
		
	/*
	if ((Empty = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1) {
		printf("semget_Empty Error\n");
		return -1;
	}
	arg.val = MAX;
	if (semctl(Empty, 0, SETVAL, arg) == -1) {
		printf("semctl_Empty Error\n");
		return -1;
	}
	if ((Mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1) {
		printf("semget_Mutex Error\n");
		return -1;
	}
	arg.val = 1;
	if (semctl(Mutex, 0, SETVAL, arg) == -1) {
		printf("semctl_Mutex Error\n");
		return -1;
	}
	if ((Full = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1) {
		printf("semget_Full Error\n");
		return -1;
	}
	arg.val = 0;
	if (semctl(Full, 0, SETVAL, arg) == -1) {
		printf("semctl_Full Error\n");
		return -1;
	}
	*/

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

/*
	if(semctl(Empty,0,IPC_RMID,arg)){
		printf("semctl_Empty_RM Error\n");
		return -1;
	}
	if(semctl(Mutex,0,IPC_RMID,arg)){
		printf("semctl_Mutex_RM Error\n");
		return -1;
	}
	if(semctl(Full,0,IPC_RMID,arg)){
		printf("semctl_Full_RM Error\n");
		return -1;
	}
	*/

	//sem_destroy(&Empty);
	//sem_destroy(&Full);
	//sem_destroy(&Mutex);
	time = (end.tv_sec-begin.tv_sec) * 1000 + (double)(end.tv_nsec-begin.tv_nsec) / 1000000;
	// time = (double)time/1000000;

	printf("time : %f\n", time/N);
	return 0;
}