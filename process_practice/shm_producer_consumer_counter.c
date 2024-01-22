#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 4
#define SHM_KEY 0x1003 // 用來辨識shared memory segment

struct shmseg{
	int in;
	int out;
	int buf[BUF_SIZE];
	int counter;
	int produce_done;
};

// counter部份還沒考慮synchronization

void producer(){
	int shmid;
	struct shmseg *shmp;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if(shmid == -1){
		perror("Shared memory");
		// return 1;
		exit(1);
	}

	// Attach to the segment to get a pointer to it
	shmp = shmat(shmid, NULL, 0);
	if(shmp == (void *)-1){
		perror("Shared memory attach");
		// return 1;
		exit(1);
	}

	printf("producer\n");
	printf("producer sizeof(*shmp): %lu\n",sizeof(*shmp));
	printf("producer shmp addr: %p\n", shmp);

	// 為什麼好像沒有自動初始化？
	// 如果要自己初始化,consumer先跑怎麼辦？ 會來不及等到producer初始化？
	shmp->in = 0;
	shmp->out = 0;
	shmp->counter = 0;
	shmp->produce_done = 0;

	printf("producer in %d out %d\n", shmp->in, shmp->out);

	int produce_list[] = {1,2,3,4,5,6,7,8,9,10};
	int produce_idx = 0;
	int produce_val;
	int max_idx = sizeof(produce_list)/sizeof(*produce_list);

	while(1){
		while(shmp->counter == BUF_SIZE);

		produce_val = produce_list[produce_idx++];
		shmp->buf[shmp->in] = produce_val;
		printf("produce val %d\n", produce_val);
		shmp->counter++;
		printf("after produce counter %d\n", shmp->counter);

		shmp->in = (shmp->in+1)%BUF_SIZE;

		if(produce_idx == max_idx){
			shmp->produce_done = 1;
			break;
		}
	}

	printf("produce done\n");

	if(shmdt(shmp) == -1){
		perror("shmdt");
		// return 1;
		exit(1);
	}
}

void consumer(){
	int shmid;
	struct shmseg *shmp;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if(shmid == -1){
		perror("Shared memory");
		// return 1;
		exit(1);
	}

	// Attach to the segment to get a pointer to it
	shmp = shmat(shmid, NULL, 0);
	if(shmp == (void *)-1){
		perror("Shared memory attach");
		// return 1;
		exit(1);
	}

	printf("consumer\n");
	printf("consumer sizeof(*shmp): %lu\n",sizeof(*shmp));
	printf("consumer shmp addr: %p\n", shmp);

	int consume_val;

	while(1){
		while(shmp->counter == 0);

		consume_val = shmp->buf[shmp->out];
		printf("consume val %d\n", consume_val);
		shmp->counter--;
		printf("after consume counter %d\n", shmp->counter);

		shmp->out = (shmp->out+1)%BUF_SIZE;

		if(shmp->produce_done && shmp->in == shmp->out){
			break;
		}
	}

	printf("consume done\n");

	if(shmdt(shmp) == -1){
		perror("shmdt");
		// return 1;
		exit(1);
	}
}

int main(int argc, char *argv[]){

	int id1 = fork();

	if(id1<0){
		printf("error in fork\n");
	}
	else if(id1 == 0){
		consumer();
	}
	else{
		producer();
	}

	return 0;
}