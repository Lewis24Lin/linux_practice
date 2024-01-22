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

	printf("consumer\n");
	printf("consumer sizeof(*shmp): %lu\n",sizeof(*shmp));
	printf("consumer shmp addr: %p\n", shmp);

	int consume_val;

	while(1){
		while(shmp->in == shmp->out);

		consume_val = shmp->buf[shmp->out];
		printf("consume val %d\n", consume_val);

		shmp->out = (shmp->out+1)%BUF_SIZE;

		if(shmp->produce_done && shmp->in == shmp->out){
			break;
		}
	}

	printf("consume done\n");

	if(shmdt(shmp) == -1){
		perror("shmdt");
		return 1;
		// exit(1);
	}

	return 0;
}