#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define THREAD_NUM 2
#define BUF_SIZE 4

// gcc thread_sameprocess_IPC_pipe.c -lpthread

// pipe

struct args{
	int th_num;
	int pipefds1[2];
	int pipefds2[2];
};

void *myThread(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("thread_num: %d\n", inputs->th_num);

	if(inputs->th_num == 0){ // sender
		char writemessage[] = "Hi";
		char readmessage[20];

		// 是不是這些IPC還是當成process層級的東西，只是只有自己process底下的therads在用
    	// 所以其中一個thread想關掉pipe的write端，另外一個thread會也沒辦法write，因為是以process的角度關掉

		// 如果pipefds寫在global 關掉會讓另一個人無法做事
		// 那要怎麼關?
		// close(pipefds1[0]);
		// close(pipefds2[1]);

		// 透過參數傳給thread再關掉也不行
		int pipefds1[2];
		int pipefds2[2];

		pipefds1[0] = inputs->pipefds1[0];
		pipefds1[1] = inputs->pipefds1[1];
		pipefds2[0] = inputs->pipefds2[0];
		pipefds2[1] = inputs->pipefds2[1];

		// close(pipefds1[0]);
		// close(pipefds2[1]);

		printf("sender %d %d %d %d\n", pipefds1[0],pipefds1[1],pipefds2[0],pipefds2[1]);

		printf("sender send message: %s\n", writemessage);
		write(pipefds1[1], writemessage, sizeof(writemessage));

		read(pipefds2[0], readmessage, sizeof(readmessage));
		printf("sender read message: %s\n", readmessage);
	}
	else if(inputs->th_num == 1){ // receiver
		char writemessage[] = "Hello";
		char readmessage[20];

		// close(pipefds1[1]);
		// close(pipefds2[0]);

		int pipefds1[2];
		int pipefds2[2];

		pipefds1[0] = inputs->pipefds1[0];
		pipefds1[1] = inputs->pipefds1[1];
		pipefds2[0] = inputs->pipefds2[0];
		pipefds2[1] = inputs->pipefds2[1];

		// close(pipefds1[1]);
		// close(pipefds2[0]);

		printf("receiver %d %d %d %d\n", pipefds1[0],pipefds1[1],pipefds2[0],pipefds2[1]);

		read(pipefds1[0], readmessage, sizeof(readmessage));
		printf("receiver read message: %s\n", readmessage);

		printf("receiver send message: %s\n", writemessage);
		write(pipefds2[1], writemessage, sizeof(writemessage));
	}
	
	
	pthread_exit(NULL);
	// return NULL;
}

int main()
{
	printf("Start\n\n");

	int i;

	pthread_t thread_id[THREAD_NUM];

	int pipefds1[2];
	int pipefds2[2];

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

	struct args inpus[THREAD_NUM];

	for(i=0;i<THREAD_NUM;i++){

		inpus[i].th_num = i;
		inpus[i].pipefds1[0] = pipefds1[0];
		inpus[i].pipefds1[1] = pipefds1[1];
		inpus[i].pipefds2[0] = pipefds2[0];
		inpus[i].pipefds2[1] = pipefds2[1];

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
	
	printf("End\n");

	pthread_exit(NULL);
	// return 0;
}
