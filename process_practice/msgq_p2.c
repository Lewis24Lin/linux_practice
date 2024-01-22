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

int main(int argc, char *argv[]){

	int msg_id;

	msg_id = msgget(MSG_KEY, 0644|IPC_CREAT);

	if(msg_id == -1){
		perror("msgget error");
		return 1;
	}

	int snd_status, rcv_status;

	struct msg_form msg;

	char message[2][MSG_MAXSIZE] = {"Hi", "Hello"};

	char msg_base[MSG_MAXSIZE] = "Child say ";

	long mtype = 1; // 決定怎麼收message
					// 0: 回傳message queue中第一個message
	                // >0: 回傳message queue中msg type(struct中的mtype) == mtype的第一個message
					// <0: 回傳message queue中msg type(struct中的mtype) <= |mtype|的第一個message, 有不同msg type, 回傳msg type最小的

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	if(rcv_status == -1){
		perror("msgrcv error");
		return 1;
	}
	printf("Child, receive message: %s\n", msg.mtext);

	msg.mtype = 2;
	strcpy(msg.mtext, msg_base);
	strcat(msg.mtext, message[0]);

	printf("Child, send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);

	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext), mtype, 0);
	printf("Child, receive message: %s\n", msg.mtext);

	msg.mtype = 2;
	strcpy(msg.mtext, msg_base);
	strcat(msg.mtext, message[1]);

	printf("Child, send message: %s\n", msg.mtext);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext), 0);


	// integer
	rcv_status = msgrcv(msg_id, &msg, sizeof(msg.mtext)+sizeof(msg.mtext_int), mtype, 0);
	printf("Child, receive int: %d\n", msg.mtext_int);

	msg.mtype = 2;
	msg.mtext_int = 12;

	printf("Chils, send int: %d\n", msg.mtext_int);
	snd_status = msgsnd(msg_id, &msg, sizeof(msg.mtext)+sizeof(msg.mtext_int), 0);

	printf("End of child\n");

	return 0;
}