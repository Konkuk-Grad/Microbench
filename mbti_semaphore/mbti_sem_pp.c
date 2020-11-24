#include "mbti_sem.h"

void sem_put_item(char local){
    sem_buffer = local;
	DEBUGMSG("sem_put_item is called\n");
}

void sem_consume_item(char* local){
	*local = sem_buffer;
	DEBUGMSG("sem_consume_item is called\n");
}

void make_shm()//프로세스간 공유 메모리를 생성한다.
{    
    if ( -1 == ( shm_id = shmget( (key_t)shm_key, shm_size, IPC_CREAT|0666)))
    {
        PRINTERROR("SHMGET ERROR");
    }

    if ( ( void *)-1 == ( shm_addr = shmat( shm_id, ( void *)0, 0)))
    {
        PRINTERROR("SHMAT ERROR");
    }
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
		DEBUGMSG("CPU_SET %d is added at cpuset\n", j);
	}   
	
	result = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    if(result != 0)//쓰레드 별로 할당 되지 않았다면 오류 출력
	{
        PRINTERROR("pid [%d] pthread_setaffinity_np failed!\n", getpid());
    }

}

void* sem_producer(void* arg)//생산자 쓰레드실행 함수, 3가지의 세마포어를 이용하여 값을 생산하고 버퍼에 추가한다.
{
	int num_cpus = *((int*)arg);
	char sem_local = 0;
	sem_set_core_affinities(num_cpus);
    clock_gettime(CLOCK_MONOTONIC,&sem_begin);
	//생산자가 공유 버퍼에 값을 집어 넣으면서 시작.
	for(int i=0;i<sem_user_iter;i++)
    {
		sem_wait(&sem_full1);
		sem_put_item(sem_local);
		sem_post(&sem_full2);
		sem_wait(&sem_full1);
		sem_consume_item(&sem_local);
		sem_post(&sem_full1);
	}
	clock_gettime(CLOCK_MONOTONIC,&sem_end);
	//생산자의 역할을 마치고 시간측정 종료, 뒤는 소비자가 가져가기만 할 뿐 시그널의 전달x라고 판단
	return 0;
}

void* sem_consumer(void* arg)//소비자 쓰레드, 3가지 세마포어를 사용하여 버퍼에 있는 아이템을 가져온다.
{
	int num_cpus = *((int*)arg);
	char sem_local = 0;
	sem_set_core_affinities(num_cpus);
    for(int i=0;i<sem_user_iter;i++) 
    {
		sem_wait(&sem_full2);
		sem_consume_item(&sem_local);
		sem_put_item(sem_local);
		sem_post(&sem_full1);
	}
	return 0;
}

double sem_iter_exec(int iter,int num_cpus)//테스트 실행
{
    pthread_t thread1;
	pthread_t thread2;
	double time;

	sem_user_iter = iter;

	if((sem_init(&sem_full1,0,1))!=0)
	{
		PRINTERROR("sem_init_full1 Error\n");
		return -1;
	}
	if((sem_init(&sem_full2,0,0))!=0)
	{
		PRINTERROR("sem_init_full2 Error\n");
		return -1;
	}

	if((pthread_create(&thread1, NULL, sem_consumer, (void *)&num_cpus))!=0)
		PRINTERROR("pthread_create thread1 is error\n");
	if((pthread_create(&thread2, NULL, sem_producer, (void *)&num_cpus))!=0)
		PRINTERROR("pthread_create thread2 is error\n");
	if((pthread_join(thread1, NULL))!=0)
		PRINTERROR("pthread_join thread1 is error\n");
	if((pthread_join(thread2, NULL))!=0)
		PRINTERROR("pthread_join thread2 is error\n");
	if((sem_destroy(&sem_full1))==-1)
		PRINTERROR("sem_full1 is not destroyed\n");
	if((sem_destroy(&sem_full2))==-1)
		PRINTERROR("sem_full2 is not destroyed\n");


	time = (sem_end.tv_sec-sem_begin.tv_sec) * 1000 + (double)(sem_end.tv_nsec-sem_begin.tv_nsec) / 1000000;
	if(time==0)
		PRINTERROR("time is null by sem_iter_exec\n");
	return time;
}

double sem_make_processes(int processes, int iter,int num_cpus)//테스트 되는 프로세스 수만큼 생성 및 실행
{
	pid_t* pid;
	pid = (pid_t*)malloc(sizeof(pid)*processes);
	if(pid == NULL)
		PRINTERROR("malloc ERROR");

    double *time, result;
	int count = 0;//생성된 프로세스의 수를 세는 카운터
    make_shm();

	for (int i = 0 ; i < processes ; i++)
    {
        if((pid[i] = fork()) < 0)
        {
            PRINTERROR("fork error");
			return -1;
        }else if(pid[i] == 0)
        {
            time = (double*)shm_addr;
            *(time+i) = sem_iter_exec(iter,num_cpus);
			if((*(time+i))==0)
				PRINTERROR("sem_iter_exec no.%d is not perfomed\n",i+1);
			exit(0);
        }else
		{
			count++;
		}
		
	}

	if(count!=processes)
		PRINTERROR("processes are not createad! They must be created %d more\n",(processes-count));

	for(int i=0;i<processes;i++)
	{
		pid_t wait_pid;
		int status;

		while((((wait_pid = wait(&status)) == -1) && errno == EINTR));	
		//느린 시스템콜로 인해 비정상 종료되는 상황을 방지

		if(WIFEXITED(status))
		{
			result += *((double*)shm_addr+i);
			DEBUGMSG("Wait() Child %d END : statue NO%d\n",i+1,WEXITSTATUS(status));
		}
		else if(WIFSIGNALED(status))
		{
			PRINTERROR("Wait() Child %d ERROR : NO%d\n",i,WTERMSIG(status));
		}
        
	}

	free(pid);
	result = result/processes;
	if(result==0)
		PRINTERROR("total result value error\n");
    return result;
}
