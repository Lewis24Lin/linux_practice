#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 2

// gcc thread_semaphore.c -lpthread -lrt

sem_t sem1;

struct args{
	int th_num;
};

void *myThread(void *arg)
{

	sem_wait(&sem1);

	struct args *inputs = (struct args *)arg;

	printf("thread_num: %d start\n", inputs->th_num);

	sleep(2);

	printf("thread_num: %d end\n", inputs->th_num);

	sem_post(&sem1);
	// printf("exit\n");
	
	pthread_exit(NULL);
	// return NULL;
}

int main()
{
	printf("Start\n\n");

	int i;

	int ret;

	pthread_t thread_id[THREAD_NUM];

	// sem_init(sem_t *sem, int pshared, unsigned int val);
	// sem: specify the semaphore to be initialized
	// pshared: newly initialized semaphore is shared between processes of between pthread_mutex_destroy
	//          zero: between threads
	//          non_zero: between processes
	// val: the value assigned to the newly initialized semaphore

	ret = sem_init(&sem1, 0, 1);
	if(ret!=0){
		printf("semahore init failed");
		return 1;
	}

	struct args inpus[THREAD_NUM];

	for(i=0;i<THREAD_NUM;i++){

		inpus[i].th_num = i;

		ret = pthread_create(&thread_id[i], NULL, myThread, &inpus[i]);
		if(ret!=0){
			printf("create thread fail");
			return 1;
		}
	}

	// pthread_join沒有寫在上面迴圈內,所以create不會等別的thread結束
	for(i=0;i<THREAD_NUM;i++){
		pthread_join(thread_id[i], NULL);
	}
	
	ret = sem_destroy(&sem1);
	if(ret!=0){
		printf("semaphore destroy failed");
		return 1;
	}

	printf("End\n");

	pthread_exit(NULL);
	// return 0;
}
