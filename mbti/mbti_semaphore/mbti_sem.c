#include "mbti_sem.h"

int sem_put_item()//공유 버퍼에 생상한 아이템을 넣는다.
{
    int item = rand()%50;
    sem_rear = (sem_rear + 1) % 1;
	sem_buffer[sem_rear] = item;
}

int sem_consume_item()//공유 버퍼에 있던 아이템을 가져온다.
{
    int item;
	item = sem_buffer[sem_front];
	sem_front = (sem_front + 1) % 1;
	return item;
}

void* sem_producer(void* arg)//생산자 쓰레드실행 함수, 3가지의 세마포어를 이용하여 값을 생산하고 버퍼에 추가한다.
{
    clock_gettime(CLOCK_MONOTONIC,&sem_begin);
	printf("begin time : %ldns\n",sem_begin.tv_nsec);
	for(int i=0;i<sem_user_iter;i++){
		sem_wait(&sem_empty); 
		sem_wait(&sem_mutex);
		sem_put_item();
		sem_post(&sem_mutex);
		sem_post(&sem_full);
	}
	printf("P is over\n");
	clock_gettime(CLOCK_MONOTONIC,&sem_end);
	printf("end time : {%4.6ld}ns\n",sem_end.tv_nsec);
	return 0;
}

void* sem_consumer(void* arg)//소비자 쓰레드, 3가지 세마포어를 사용하여 버퍼에 있는 아이템을 가져온다.
{
    for(int i=0;i<sem_user_iter;i++) {
		sem_wait(&sem_full);
		sem_wait(&sem_mutex);
		sem_consume_item();
		sem_post(&sem_mutex);
		sem_post(&sem_empty);
	}
	printf("C is over\n");
	return 0;
}

double sem_test_exec(int topology, int processes, int iter, int num_cpus)//테스트 실행
{
    pthread_t thread1;
	pthread_t thread2;
	double time;

	sem_user_iter = iter;

	if((sem_init(&sem_empty,0,1))!=0)
	{
		printf("sem_init_Empty Error\n");
		return -1;
	}
	if((sem_init(&sem_mutex,0,1))!=0)
	{
		printf("sem_init_Mutex Error\n");
		return -1;
	}
	if((sem_init(&sem_full,0,0))!=0)
	{
		printf("sem_init_Full Error\n");
		return -1;
	}

	pthread_create(&thread1, NULL, sem_consumer, NULL);
	pthread_create(&thread2, NULL, sem_producer, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	sem_destroy(&sem_empty);
	sem_destroy(&sem_full);
	sem_destroy(&sem_mutex);
	time = (sem_end.tv_sec-sem_begin.tv_sec) * 1000 + (double)(sem_end.tv_nsec-sem_begin.tv_nsec) / 1000000;

	printf("time : {%4.6f}ms\n", time/iter);
	return time;
}

int main(int argc, char *argv[]) {
    int iter = atoi(argv[1]);
    sem_test_exec(0,0,iter,0);
    return 0;
}