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
#define SHM_KEY 0x1040 // 用來辨識shared memory segment

// gcc shm_mutex_semaphore.c -lpthread -lrt

// shared memory

// process之間用pthread的mutex也可以跑,這樣有沒有問題

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

void func_parent()
{
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

	ret = pthread_mutex_init(&shmp->lock, NULL);
	if(ret!=0){
		printf("mutex init failed");
		exit(1);
	}

	// 第二個參數改成1
	// 寫0好像跑起來會有問題
	ret = sem_init(&shmp->sem1, 1, 1);
	if(ret!=0){
		printf("semahore init failed");
		exit(1);
	}

	int produce_val[] = {1,2,3,4,5,6};
	int produce_size = sizeof(produce_val)/sizeof(*produce_val);
	int produce_cnt = 0;

	while(1){
		while(shmp->counter == BUF_SIZE);

		shmp->buf[shmp->in] = produce_val[produce_cnt];
		printf("parent produce val %d\n", produce_val[produce_cnt]);
		produce_cnt++;

		shmp->in = (shmp->in+1)%BUF_SIZE;

		// pthread_mutex_lock(&shmp->lock);
		sem_wait(&shmp->sem1);

		shmp->counter++;
		printf("parent after produce counter %d\n", shmp->counter);

		// pthread_mutex_unlock(&shmp->lock);
		sem_post(&shmp->sem1);

		if(produce_cnt == produce_size){
			shmp->produce_done = 1;
			break;
		}
	}

	printf("parent done\n");

	// 等待consume完才把ㄌㄧmutex,semaphore資訊清掉
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
	
}

void func_child()
{
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

	int consume_val;

	while(1){
		while(shmp->counter == 0);

		consume_val = shmp->buf[shmp->out];
		printf("child consume val %d\n", consume_val);

		shmp->out = (shmp->out+1)%BUF_SIZE;

		sem_wait(&shmp->sem1);

		shmp->counter--;
		printf("child after produce counter %d\n", shmp->counter);

		sem_post(&shmp->sem1);

		if(shmp->produce_done && shmp->counter==0){
			shmp->consume_done = 1;
			break;
		}
	}

	printf("child done\n");

	if(shmdt(shmp) == -1){
		perror("child shmdt fail");
		exit(1);
	}
	
	
}

int main()
{

	int forkid = fork();

	if(forkid < 0){
		printf("fork fail");
	}
	else if(forkid == 0){ // child
		func_child();
	}
	else{ // parent
		func_parent();
	}

	return 0;
}
