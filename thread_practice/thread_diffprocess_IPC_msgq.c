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
#define MSG_KEY 0x2020
#define MSG_MAXSIZE 64

// gcc thread_diffprocess_IPC_msgq.c -lpthread

// message queue

struct msg_form{
	long mtype; // msg type, >0
	char mtext[MSG_MAXSIZE];
	int mtext_int;
};

struct args{
	int th_num;
};

void *myThread_parent(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("parent, process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	int msg_id;
	int snd_status, rcv_status;
	struct msg_form msg;
	long mtype = 2;

	msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);
	if(msg_id == -1){
		perror("parent msgget error");
		exit(1);
	}

	printf("parent msg_id: %d\n", msg_id);

	msg.mtype = 1;
	strcpy(msg.mtext, "Hi");

	printf("parent send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);
	if(snd_status == -1){
		perror("parent msgsnd error");
		exit(1);
	}

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	printf("parent receive message: %s\n", msg.mtext);

	printf("End of parent\n");

	pthread_exit(NULL);
	// return NULL;
}

void *myThread_child(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("child process: %d, thread_num: %d\n", getpid(), inputs->th_num);

	int msg_id;
	int snd_status, rcv_status;
	struct msg_form msg;
	long mtype = 1;

	msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);
	if(msg_id == -1){
		perror("child msgget error");
		exit(1);
	}

	printf("child msg_id: %d\n", msg_id);

	msg.mtype = 2;
	strcpy(msg.mtext, "Hello");

	printf("child send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);
	if(snd_status == -1){
		perror("child msgsnd error");
		exit(1);
	}

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	printf("child receive message: %s\n", msg.mtext);

	printf("End of child\n");
	
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
