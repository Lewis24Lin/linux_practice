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

	int pipefds1[2]; // parent write, child read
	int pipefds2[2]; // child write, parent read
	int returnstatus1, returnstatus2;
	char writemessages1[2][20] = {"Hi", "Hello"};
	char writemessages2[2][20] = {"Hi from child", "Hello from child"};
	char readmessage[20];

	int writeval1 = 11;
	int writeval2 = 12;
	int readval = 0;

	returnstatus1 = pipe(pipefds1);

	if(returnstatus1 == -1){
		printf("Unable to create pipe 1\n");
		return 1;
	}

	returnstatus2 = pipe(pipefds2);

	if(returnstatus2 == -1){
		printf("Unable to create pipe 2\n");
		return 1;
	}

	int pid = fork();

	// 可改變各種讀寫順序或拿掉某寫讀寫看看結果

	if(pid == 0){ // child

		close(pipefds1[1]);
		close(pipefds2[0]);

		read(pipefds1[0], readmessage, sizeof(readmessage));
		printf("Child, Reading from pipe 1 - Message 1 is %s\n", readmessage);

		printf("Child, Writing to pipe 2 - Message 1 is %s\n", writemessages2[0]);
		write(pipefds2[1], writemessages2[0], sizeof(writemessages2[0]));

		read(pipefds1[0], readmessage, sizeof(readmessage));
		printf("Child, Reading from pipe 1 - Message 2 is %s\n", readmessage);

		printf("Child, Writing to pipe 2 - Message 2 is %s\n", writemessages2[1]);
		write(pipefds2[1], writemessages2[1], sizeof(writemessages2[1]));


		// integer	
		read(pipefds1[0], &readval, sizeof(readval));
		printf("Child, Reading int from pipe 1, val = %d\n", readval);

		printf("Child, Writing int to pipe 2, val = %d\n", writeval2);
		write(pipefds2[1], &writeval2, sizeof(writeval2));

		printf("End of child\n");
	}
	else{ // parent

		close(pipefds1[0]);
		close(pipefds2[1]);

		// printf("Parent, input 1: ");
		// scanf("%s", writemessages1[0]);
		printf("Parent, Writing to pipe 1 - Message 1 is %s\n", writemessages1[0]);
		write(pipefds1[1], writemessages1[0], sizeof(writemessages1[0]));

		read(pipefds2[0], readmessage, sizeof(readmessage));
		printf("Parent, Reading from pipe 2 - Message 1 is %s\n", readmessage);

		// printf("Parent, input 2: ");
		// scanf("%s", writemessages1[1]);
		printf("Parent, Writing to pipe 1 - Message 2 is %s\n", writemessages1[1]);
		write(pipefds1[1], writemessages1[1], sizeof(writemessages1[1]));

		read(pipefds2[0], readmessage, sizeof(readmessage));
		printf("Parent, Reading from pipe 2 - Message 2 is %s\n", readmessage);


		// integer
		printf("Parent, Writing int to pipe 1, val = %d\n", writeval1);
		write(pipefds1[1], &writeval1, sizeof(writeval1));

		read(pipefds2[0], &readval, sizeof(readval));
		printf("Parent, Reading int from pipe 2, val = %d\n", readval);

		// 寫了但沒人接,也可以往後走 (non-blocking的概念?)
		printf("Parent, Writing to pipe 1 - Message 1 is %s\n", writemessages1[0]);
		write(pipefds1[1], writemessages1[0], sizeof(writemessages1[0]));

		printf("End of parent\n");
	}

	return 0;
}