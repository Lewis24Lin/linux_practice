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
#define SHM_KEY 0x1002 // 用來辨識shared memory segment

struct shmseg{
	int in;
	int out;
	int buf[BUF_SIZE];
	int produce_done;
};

int main(int argc, char *argv[]){

	int shmid;
	struct shmseg *shmp;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if(shmid == -1){
		perror("Shared memory");
		return 1;
		// exit(1);
	}

	// Attach to the segment to get a pointer to it
	shmp = shmat(shmid, NULL, 0);
	if(shmp == (void *)-1){
		perror("Shared memory attach");
		return 1;
		// exit(1);
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
		return 1;
		// exit(1);
	}

	return 0;
}