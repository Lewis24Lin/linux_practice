#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 2
#define BUF_SIZE 4

// gcc thread_producer_consumer_counter_mutex.c -lpthread

int buffer[BUF_SIZE];
int in;
int out;
int produce_done = 0;

int counter = 0;

pthread_mutex_t lock;

struct args{
	int th_num;
};


void *myThread(void *arg)
{
	struct args *inputs = (struct args *)arg;

	printf("thread_num: %d\n", inputs->th_num);

	if(inputs->th_num == 0){ // producer
		int produce_items[] = {1,2,3,4,5,6,7,8,9,10};
		int size = sizeof(produce_items)/sizeof(*produce_items);
		int idx = 0;

		while(1){
			while(counter == BUF_SIZE);

			int produce_val = produce_items[idx++];
			buffer[in] = produce_val;
			printf("produce val: %d\n", produce_val);
			in = (in+1)%BUF_SIZE;
			
			pthread_mutex_lock(&lock);
			counter++;
			printf("after produce %d, counter: %d\n", produce_val, counter);
			pthread_mutex_unlock(&lock);

			if(idx==size){
				produce_done = 1;
				break;
			}
		}

	}
	else if(inputs->th_num == 1){ // consumer
		int consume_val;

		while(1){
			while(counter == 0);

			consume_val = buffer[out];
			printf("consume val: %d\n", consume_val);
			out = (out+1)%BUF_SIZE;

			pthread_mutex_lock(&lock);
			counter--;
			printf("after consume %d, counter: %d\n", consume_val, counter);
			pthread_mutex_unlock(&lock);

			if(produce_done && counter==0){
				break;
			}
		}
	}
	
	
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
	
	ret = pthread_mutex_destroy(&lock);
	if(ret!=0){
		printf("mutex destroy failed");
		return 1;
	}

	printf("End\n");

	pthread_exit(NULL);
	// return 0;
}
