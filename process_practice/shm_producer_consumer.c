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
#define SHM_KEY 0x1001 // 用來辨識shared memory segment

// 目前是手動指定key
// ftok寫法可以用來產生key

struct shmseg{
	int in;
	int out;
	int buf[BUF_SIZE];
	int produce_done;
};

void producer(){
	int shmid;
	struct shmseg *shmp;

	// shmget建立的空間會初始化?
	// shmget會辨識key,沒有key就建立新的segment,有key就拿現有的segment
	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT); // 0644應該是user,group,other的權限
	if(shmid == -1){
		perror("Shared memory");
		// return 1;
		exit(1);
	}

	printf("producer shmid id: %d\n", shmid);

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

	shmp->in = 0;
	shmp->out = 0;
	shmp->produce_done = 0;

	printf("producer in %d out %d\n", shmp->in, shmp->out);

	int produce_list[] = {1,2,3,4,5,6,7,8,9,10};
	int produce_idx = 0;
	int produce_val;
	int max_idx = sizeof(produce_list)/sizeof(*produce_list);

	while(1){
		while((shmp->in+1)%BUF_SIZE == shmp->out);

		produce_val = produce_list[produce_idx++];
		shmp->buf[shmp->in] = produce_val;
		printf("produce val %d\n", produce_val);

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

	printf("consumer shmid id: %d\n", shmid);

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
		while(shmp->in == shmp->out);

		consume_val = shmp->buf[shmp->out];
		printf("consume val %d\n", consume_val);

		shmp->out = (shmp->out+1)%BUF_SIZE;

		// producer做完後, consumer把剩下的東西consume完就結束
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