#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

int main(){

	int pipefds[2];
	int returnstatus;
	char writemessages[4][20] = {"Hi", "Hello", "Hi from child", "Hello from child"};
	char readmessage[20];

	returnstatus = pipe(pipefds);

	if(returnstatus == -1){
		printf("Unable to create pipe\n");
		return 1;
	}

	int pid = fork();

	if(pid == 0){ // child
		// printf("child pid: %d\n",getpid());
		// printf("child fork return: %d\n",pid);

		read(pipefds[0], readmessage, sizeof(readmessage));
		printf("Child, Reading from pipe - Message 1 is %s\n", readmessage);

		read(pipefds[0], readmessage, sizeof(readmessage));
		printf("Child, Reading from pipe - Message 2 is %s\n", readmessage);

		printf("Child, Writing to pipe - Message 3 is %s\n", writemessages[2]);
		write(pipefds[1], writemessages[2], sizeof(writemessages[2]));
	}
	else{ // parent
		// printf("parent pid:%d\n",getpid());
		// printf("parent fork return: %d\n",pid);

		printf("Parent, Writing to pipe - Message 1 is %s\n", writemessages[0]);
		write(pipefds[1], writemessages[0], sizeof(writemessages[0]));

		printf("Parent, Writing to pipe - Message 2 is %s\n", writemessages[1]);
		write(pipefds[1], writemessages[1], sizeof(writemessages[1]));
	
		// 這樣用同一個pipe讀寫, 可能自己寫進去的自己讀出來
		// child就讀不到parent要給他的東西, 後續的動作也不會做？

		read(pipefds[0], readmessage, sizeof(readmessage));
		printf("Parent, Reading from pipe - Message 3 is %s\n", readmessage);
	}

	return 0;
}