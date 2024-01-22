#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define THREAD_NUM 2
#define BUF_SIZE 4
#define SHM_KEY 0x1010 // 用來辨識shared memory segment

// gcc thread_sameprocess_IPC_shm.c -lpthread
// gcc thread_sameprocess_IPC_shm.c -lpthread -lrt

// shared memory

struct shmseg{
	int in;
	int out;
	int buf[BUF_SIZE];
	int counter;
	int produce_done;
};

struct args{
	int th_num;
};

pthread_mutex_t lock;

sem_t sem1;

void *myThread(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("thread_num: %d\n", inputs->th_num);

	if(inputs->th_num == 0){ // producer
		int shmid;
		struct shmseg *shmp;

		shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
		if(shmid == -1){
			perror("producer shmget fail");
			exit(1);
		}

		printf("producer shmid: %d\n", shmid);

		// Attach to the segment to get a pointer to it
		shmp = shmat(shmid, NULL, 0);
		if(shmp == (void *)-1){
			perror("producer shmat fail");
			exit(1);
		}

		int produce_val;

		// 只寫一個值,只是要測process內的thread能不能用IPC的東西
		// 只寫一個值,所以counter看不出異常,因為consumer一定會等producer寫了那個值才開始讀
		while(1){
			while(shmp->counter == BUF_SIZE);

			produce_val = 1000;
			shmp->buf[shmp->in] = produce_val;
			printf("produce val %d\n", produce_val);

			shmp->in = (shmp->in+1)%BUF_SIZE;

			pthread_mutex_lock(&lock);
			// sem_wait(&sem1);

			shmp->counter++;
			printf("producer counter %d\n", shmp->counter);

			pthread_mutex_unlock(&lock);
			// sem_post(&sem1);

			shmp->produce_done = 1;
			break;
		}

		printf("produce done\n");

		if(shmdt(shmp) == -1){
			perror("producer shmdt fail");
			exit(1);
		}
	}
	else if(inputs->th_num == 1){ // consumer
		int shmid;
		struct shmseg *shmp;

		shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
		if(shmid == -1){
			perror("consumer shmget fail");
			exit(1);
		}

		printf("consumer shm id: %d\n", shmid);

		// Attach to the segment to get a pointer to it
		shmp = shmat(shmid, NULL, 0);
		if(shmp == (void *)-1){
			perror("consumer shmat fail");
			exit(1);
		}

		int consume_val;

		while(1){
			while(shmp->counter == 0);

			consume_val = shmp->buf[shmp->out];
			printf("consume val %d\n", consume_val);

			shmp->out = (shmp->out+1)%BUF_SIZE;

			pthread_mutex_lock(&lock);
			// sem_wait(&sem1);

			shmp->counter--;
			printf("consumer counter %d\n", shmp->counter);

			pthread_mutex_unlock(&lock);
			// sem_post(&sem1);

			if(shmp->produce_done && shmp->counter==0){
				break;
			}
		}

		printf("consume done\n");

		if(shmdt(shmp) == -1){
			perror("consumer shmdt fail");
			exit(1);
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

		// pthread_join(thread_id[i], NULL);
	}

	// pthread_join沒有寫在上面迴圈內,所以create不會等別的job結束
	for(i=0;i<THREAD_NUM;i++){
		pthread_join(thread_id[i], NULL);
	}

	ret = pthread_mutex_destroy(&lock);
	if(ret!=0){
		printf("mutex destroy failed");
		return 1;
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
