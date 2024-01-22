#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 2

// gcc thread_mutex.c -lpthread

int counter = 0;
pthread_mutex_t lock;

void *myThread(void *arg)
{

	pthread_mutex_lock(&lock);

	unsigned long i = 0;

	counter++;
	printf("Job %d has started\n", counter);

	// 這樣會讓job 1跑很久,在job 1結束前,job 2就開始並且把counter+1
	for(i=0;i<0xFFFFFFF;i++); // 0xFFFFFFFF

	printf("Job %d has finished\n", counter);

	pthread_mutex_unlock(&lock);
	
	pthread_exit(NULL);
	// return NULL;
}

int main()
{
	printf("Start\n\n");

	int i;

	pthread_t thread_id[THREAD_NUM];

	int ret;

	ret = pthread_mutex_init(&lock, NULL);
	if(ret!=0){
		printf("mutex init failed");
		return 1;
	}

	for(i=0;i<THREAD_NUM;i++){

		ret = pthread_create(&thread_id[i], NULL, myThread, NULL);
		if(ret!=0){
			printf("create thread fail");
			return 1;
		}
		
	}

	// pthread_join沒有寫在上面迴圈內,所以create不會等別的thread結束
	for(i=0;i<THREAD_NUM;i++){
		pthread_join(thread_id[i], NULL);
	}
	
	ret = pthread_mutex_destroy(&lock);
	if(ret!=0){
		printf("mutex destroy failed");
		return 1;
	}

	printf("End\n");

	pthread_exit(NULL);
	// return 0;
}
