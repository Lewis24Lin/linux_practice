#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#define MSG_KEY 0x2002
#define MSG_MAXSIZE 64

struct msg_form{
	long mtype; // msg type, >0
	char mtext[MSG_MAXSIZE];
	int mtext_int;
};

// 現在寫法是在背景執行兩支程式,不是用fork()+execl()

int main(int argc, char *argv[]){

	int msg_id;

	msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);

	if(msg_id == -1){
		perror("msgget error");
		return 1;
	}

	int snd_status, rcv_status;

	struct msg_form msg;

	// char message[2][MSG_MAXSIZE] = {"Hi", "Hello"};
	// sprintf(msg.mtext, "Parent say %s", message[0]); // 為什麼這樣不行？ 會什麼好像會算整個message[0]的size而不是"Hi"的size? 

	char message[2][MSG_MAXSIZE] = {"Hi", "Hello"};

	char msg_base[MSG_MAXSIZE] = "Parent say ";

	long mtype = 2;

	msg.mtype = 1;
	strcpy(msg.mtext, msg_base);
	strcat(msg.mtext, message[0]);

	printf("Parent, send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);
	if(snd_status == -1){
		perror("msgsnd error");
		return 1;
	}

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	printf("Parent, receive message: %s\n", msg.mtext);

	msg.mtype = 1;
	strcpy(msg.mtext, msg_base);
	strcat(msg.mtext, message[1]);

	printf("Parent, send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	printf("Parent, receive message: %s\n", msg.mtext);


	// integer
	msg.mtype = 1;
	msg.mtext_int = 11;

	printf("Parent, send int: %d\n", msg.mtext_int);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext)+sizeof(msg.mtext_int), 0);

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext)+sizeof(msg.mtext_int), mtype, 0);
	printf("Parent, receive int: %d\n", msg.mtext_int);

	// 寫了但沒人接,也可以往後走
	// 只是message queue裡面就會留下沒被讀走的資料
	// msg.mtype = 1;
	// strcpy(msg.mtext, msg_base);
	// strcat(msg.mtext, message[0]);

	// printf("Parent, send message: %s\n", msg.mtext);
	// snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);

	printf("End of parent\n");

	return 0;
}