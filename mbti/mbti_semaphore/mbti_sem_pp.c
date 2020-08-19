#include "mbti_sem.h"

void make_shm()//프로세스간 공유 메모리를 생성한다.
{    
    if ( -1 == ( shm_id = shmget( (key_t)shm_key, shm_size, IPC_CREAT|0666)))
    {
        perror("SHMGET ERROR");
    }

    if ( ( void *)-1 == ( shm_addr = shmat( shm_id, ( void *)0, 0)))
    {
        perror("SHMAT ERROR");
    }
}

int sem_put_item()//공유 버퍼에 생상한 아이템을 넣는다.
{
    int item = rand()%50;
    sem_rear = (sem_rear + 1) % 1;
	sem_buffer[sem_rear] = item;
}

void sem_set_core_affinities(int num_cpus)//코어설정 함수
{
	cpu_set_t cpuset;
	int result;
	pthread_t current_thread = pthread_self();//현재 쓰레드에 cpu를 할당하기 위해 현재 쓰레드 지정

	CPU_ZERO(&cpuset);//cpu 초기화
    for (int j = 0; j < num_cpus; j++)
    {
		CPU_SET(j, &cpuset);//0~core의 수만큼 활성화되도록 set에 추가
	}   
	
	result = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    if(result != 0)//쓰레드 별로 할당 되지 않았다면 오류 출력
	{
        printf("pid [%d] pthread_setaffinity_np failed!\n", getpid());
    }

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
	int num_cpus = *((int*)arg);
	sem_set_core_affinities(num_cpus);
    clock_gettime(CLOCK_MONOTONIC,&sem_begin);
	//생산자가 공유 버퍼에 값을 집어 넣으면서 시작.
	for(int i=0;i<sem_user_iter;i++)
    {
		sem_wait(&sem_empty); 
		sem_wait(&sem_mutex);
		sem_put_item();
		sem_post(&sem_mutex);
		sem_post(&sem_full);
	}
	clock_gettime(CLOCK_MONOTONIC,&sem_end);
	//생산자의 역할을 마치고 시간측정 종료, 뒤는 소비자가 가져가기만 할 뿐 시그널의 전달x라고 판단
	return 0;
}

void* sem_consumer(void* arg)//소비자 쓰레드, 3가지 세마포어를 사용하여 버퍼에 있는 아이템을 가져온다.
{
	int num_cpus = *((int*)arg);
	sem_set_core_affinities(num_cpus);
    for(int i=0;i<sem_user_iter;i++) 
    {
		sem_wait(&sem_full);
		sem_wait(&sem_mutex);
		sem_consume_item();
		sem_post(&sem_mutex);
		sem_post(&sem_empty);
	}
	return 0;
}

double sem_iter_exec(int iter,int num_cpus)//테스트 실행
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

	pthread_create(&thread1, NULL, sem_consumer, (void *)&num_cpus);
	pthread_create(&thread2, NULL, sem_producer, (void *)&num_cpus);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	sem_destroy(&sem_empty);
	sem_destroy(&sem_full);
	sem_destroy(&sem_mutex);
	time = (sem_end.tv_sec-sem_begin.tv_sec) * 1000 + (double)(sem_end.tv_nsec-sem_begin.tv_nsec) / 1000000;

	return time/iter;
}

double sem_make_processes(int processes, int iter,int num_cpus)//테스트 되는 프로세스 수만큼 생성 및 실행
{
	pid_t* pid;
	pid = (pid_t*)malloc(sizeof(pid)*processes);
	if(pid == NULL)
		perror("malloc ERROR");

    double *time;
    double result;
    make_shm();

	for (int i = 0 ; i < processes ; i++)
    {
        if((pid[i] = fork()) < 0)
        {
            perror("fork error");
        }else if(pid[i] == 0)
        {
			make_shm();
            time = (double*)shm_addr;
            *(time+i) = sem_iter_exec(iter,num_cpus);
			exit(0);
        }
	}

	for(int i=0;i<processes;i++)
	{
		pid_t wait_pid;
		int status;

		if(pid[i] > 0)
		{
			while((((wait_pid = wait(&status)) == -1) && errno == EINTR));	
			//느린 시스템콜로 인해 비정상 종료되는 상황을 방지

            if(WIFEXITED(status))
            {
                result += *((double*)shm_addr+i);
                printf("Wait() Child END : statue NO%d\n",WEXITSTATUS(status));
            }
            else if(WIFSIGNALED(status))
            {
                printf("Wait() Child %d ERROR : NO%d\n",i,WTERMSIG(status));
            }
        
		}
	}

	free(pid);

    return result/processes;
}