#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <errno.h>

#define THREAD_NUM 1
#define BUF_SIZE 4
#define SHM_KEY 0x1020 // 用來辨識shared memory segment

// gcc thread_diffprocess_IPC_shm.c -lpthread
// gcc thread_diffprocess_IPC_shm.c -lpthread -lrt

// shared memory

struct shmseg{
	int in;
	int out;
	int buf[BUF_SIZE];
	int counter;
	int produce_done;
	int consume_done;
	pthread_mutex_t lock;
	sem_t sem1;
};

struct args{
	int th_num;
};

// 不同process看到的lock是同一個lock嗎?
// lock是不是要寫在shared memory上?
// pthread_mutex_t lock;

void *myThread_parent(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("parent, process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	int shmid;
	struct shmseg *shmp;
	int ret;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if(shmid == -1){
		perror("parent shmget fail");
		exit(1);
	}

	printf("parent shmid: %d\n", shmid);

	// Attach to the segment to get a pointer to it
	shmp = shmat(shmid, NULL, 0);
	if(shmp == (void *)-1){
		perror("parent shmat fail");
		exit(1);
	}

	// mutex的初始化跟銷毀都由parent來做可以嗎?
	ret = pthread_mutex_init(&shmp->lock, NULL);
	if(ret!=0){
		printf("mutex init failed");
		exit(1);
	}

	ret = sem_init(&shmp->sem1, 0, 1);
	if(ret!=0){
		printf("semahore init failed");
		exit(1);
	}

	printf("parent lock addr: %p\n", &shmp->lock);

	int produce_val;

	while(1){
		while(shmp->counter == BUF_SIZE);

		produce_val = 1000;
		shmp->buf[shmp->in] = produce_val;
		printf("parent produce val %d\n", produce_val);

		shmp->in = (shmp->in+1)%BUF_SIZE;

		// pthread_mutex_lock(&shmp->lock);
		sem_wait(&shmp->sem1);

		shmp->counter++;
		printf("parent counter %d\n", shmp->counter);

		// pthread_mutex_unlock(&shmp->lock);
		sem_post(&shmp->sem1);

		shmp->produce_done = 1;
		break;
	}

	printf("parent done\n");

	// 等待consume完才把mutex, semaphore資訊清掉
	while(!shmp->consume_done);

	ret = pthread_mutex_destroy(&shmp->lock);
	if(ret!=0){
		printf("mutex destroy failed");
		exit(1);
	}

	ret = sem_destroy(&shmp->sem1);
	if(ret!=0){
		printf("semaphore destroy failed");
		exit(1);
	}

	if(shmdt(shmp) == -1){
		perror("parent shmdt fail");
		exit(1);
	}
	
	pthread_exit(NULL);
	// return NULL;
}

void *myThread_child(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("child process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	int shmid;
	struct shmseg *shmp;
	int ret;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if(shmid == -1){
		perror("child shmget fail");
		exit(1);
	}

	printf("child shmid: %d\n", shmid);

	// Attach to the segment to get a pointer to it
	shmp = shmat(shmid, NULL, 0);
	if(shmp == (void *)-1){
		perror("child shmat fail");
		exit(1);
	}

	// ret = pthread_mutex_init(&shmp->lock, NULL);
	// if(ret!=0){
	// 	printf("mutex init failed");
	// 	exit(1);
	// }

	printf("child lock addr: %p\n", &shmp->lock);

	int consume_val;

	while(1){
		while(shmp->counter == 0);

		consume_val = shmp->buf[shmp->out];
		printf("child consume val %d\n", consume_val);

		shmp->out = (shmp->out+1)%BUF_SIZE;

		// pthread_mutex_lock(&shmp->lock);
		sem_wait(&shmp->sem1);

		shmp->counter--;
		printf("child counter %d\n", shmp->counter);

		// pthread_mutex_unlock(&shmp->lock);
		sem_post(&shmp->sem1);

		if(shmp->produce_done && shmp->counter==0){
			shmp->consume_done = 1;
			break;
		}
	}

	printf("child done\n");

	// ret = pthread_mutex_destroy(&shmp->lock);
	// if(ret!=0){
	// 	printf("mutex destroy failed");
	// 	exit(1);
	// }

	if(shmdt(shmp) == -1){
		perror("child shmdt fail");
		exit(1);
	}
	
	pthread_exit(NULL);
	// return NULL;
}

int main()
{

	int forkid = fork();

	if(forkid < 0){
		printf("fork fail");
	}
	else if(forkid == 0){ // child
		pthread_t thread_id[THREAD_NUM];
		int ret;
		struct args inpus[THREAD_NUM];

		int i;

		for(i=0;i<THREAD_NUM;i++){

			inpus[i].th_num = i;

			ret = pthread_create(&thread_id[i], NULL, myThread_child, &inpus[i]);
			if(ret!=0){
				printf("create thread fail");
				return 1;
			}

			// pthread_join(thread_id[i], NULL);
		}

		// pthread_join沒有寫在上面迴圈內,所以create不會等別的job結束
		// for(i=0;i<THREAD_NUM;i++){
		// 	pthread_join(thread_id[i], NULL);
		// }
	}
	else{ // parent
		pthread_t thread_id[THREAD_NUM];
		int ret;
		struct args inpus[THREAD_NUM];

		int i;

		for(i=0;i<THREAD_NUM;i++){

			inpus[i].th_num = i;

			ret = pthread_create(&thread_id[i], NULL, myThread_parent, &inpus[i]);
			if(ret!=0){
				printf("create thread fail");
				return 1;
			}

			// pthread_join(thread_id[i], NULL);
		}

		// pthread_join沒有寫在上面迴圈內,所以create不會等別的job結束
		// for(i=0;i<THREAD_NUM;i++){
		// 	pthread_join(thread_id[i], NULL);
		// }
	}

	pthread_exit(NULL);
	// return 0;
}
