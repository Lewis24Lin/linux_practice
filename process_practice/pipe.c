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

	int pipefds[2]; // write to idx 1, read from idx 0
	int returnstatus;
	char writemessages[2][20] = {"Hi", "Hello"};
	char readmessage[20];

	returnstatus = pipe(pipefds);

	if(returnstatus == -1){
		printf("Unable to create pipe\n");
		return 1;
	}

	// 寫一次讀一次

	printf("Writing to pipe - Message 1 is %s\n", writemessages[0]);
	write(pipefds[1], writemessages[0], sizeof(writemessages[0]));
	read(pipefds[0], readmessage, sizeof(readmessage));
	printf("Reading from pipe - Message 1 is %s\n", readmessage);

	printf("Writing to pipe - Message 2 is %s\n", writemessages[1]);
	write(pipefds[1], writemessages[1], sizeof(writemessages[1]));
	read(pipefds[0], readmessage, sizeof(readmessage));
	printf("Reading from pipe - Message 2 is %s\n", readmessage);

	printf("\n");

	// 寫多次再依序讀出來

	printf("Writing to pipe - Message 1 is %s\n", writemessages[0]);
	write(pipefds[1], writemessages[0], sizeof(writemessages[0]));

	printf("Writing to pipe - Message 2 is %s\n", writemessages[1]);
	write(pipefds[1], writemessages[1], sizeof(writemessages[1]));

	read(pipefds[0], readmessage, sizeof(readmessage));
	printf("Reading from pipe - Message 1 is %s\n", readmessage);

	read(pipefds[0], readmessage, sizeof(readmessage));
	printf("Reading from pipe - Message 2 is %s\n", readmessage);

	return 0;
}