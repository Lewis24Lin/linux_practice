#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define THREAD_NUM 2
#define MSG_KEY 0x2010
#define MSG_MAXSIZE 64

// gcc thread_sameprocess_IPC_msgq.c -lpthread

// message queue

struct msg_form{
	long mtype; // msg type, >0
	char mtext[MSG_MAXSIZE];
	int mtext_int;
};

struct args{
	int th_num;
};

void *myThread(void *arg)
{

	struct args *inputs = (struct args *)arg;

	printf("thread_num: %d\n", inputs->th_num);

	if(inputs->th_num == 0){ // sender
		int msg_id;
		int snd_status, rcv_status;
		struct msg_form msg;
		long mtype = 2;

		msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);
		if(msg_id == -1){
			perror("msgget error");
			exit(1);
		}

		printf("sender msg_id: %d\n", msg_id);

		msg.mtype = 1;
		strcpy(msg.mtext, "Hi");

		printf("sender send message: %s\n", msg.mtext);
		snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);
		if(snd_status == -1){
			perror("msgsnd error");
			exit(1);
		}

		rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
		printf("sender receive message: %s\n", msg.mtext);

		printf("End of sender\n");
	}
	else if(inputs->th_num == 1){ // receiver
		int msg_id;
		int snd_status, rcv_status;
		struct msg_form msg;
		long mtype = 1;

		msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);
		if(msg_id == -1){
			perror("msgget error");
			exit(1);
		}

		printf("receiver msg_id: %d\n", msg_id);

		rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
		if(rcv_status == -1){
			perror("msgrcv error");
			exit(1);
		}
		printf("receiver receive message: %s\n", msg.mtext);

		msg.mtype = 2;
		strcpy(msg.mtext, "Hello");

		printf("receiver send message: %s\n", msg.mtext);
		snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);

		printf("End of receiver\n");
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
	
	printf("End\n");

	pthread_exit(NULL);
	// return 0;
}
