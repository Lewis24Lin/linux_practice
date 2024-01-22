#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <errno.h>

#define THREAD_NUM 1
#define BUF_SIZE 4

// gcc thread_diffprocess_IPC_pipe.c -lpthread

// pipe

struct args{
	int th_num;
};

int pipefds1[2];
int pipefds2[2];

void *myThread_parent(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("parent, process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	char writemessage[] = "Hi";
	char readmessage[20];

	close(pipefds1[0]);
	close(pipefds2[1]);

	printf("parent send message: %s\n", writemessage);
	write(pipefds1[1], writemessage, sizeof(writemessage));

	read(pipefds2[0], readmessage, sizeof(readmessage));
	printf("parent read message: %s\n", readmessage);
	
	pthread_exit(NULL);
	// return NULL;
}

void *myThread_child(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("child process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	char writemessage[] = "Hello";
	char readmessage[20];

	close(pipefds1[1]);
	close(pipefds2[0]);

	read(pipefds1[0], readmessage, sizeof(readmessage));
	printf("child read message: %s\n", readmessage);

	printf("child send message: %s\n", writemessage);
	write(pipefds2[1], writemessage, sizeof(writemessage));
	
	pthread_exit(NULL);
	// return NULL;
}

int main()
{

	int ret;

	ret = pipe(pipefds1);
	if(ret == -1){
		printf("Unable to create pipe 1\n");
		return 1;
	}

	ret = pipe(pipefds2);
	if(ret == -1){
		printf("Unable to create pipe 2\n");
		return 1;
	}

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
