#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#define NUM_PIPES 2

int main(){

	int i;

	int pipefds[NUM_PIPES*2]; // 0:child read, 1:parent write, 2:parent read, 3:child write
	int returnstatus;
	
	for(i=0;i<NUM_PIPES;i++){
		returnstatus = pipe(pipefds+2*i);
		if(returnstatus == -1){
			perror("Failed to allocate the pipe\n");
			return 1;
		}
	}

	int pid = fork();

	if(pid < 0){
		perror("Failed to fork process\n");
	}
	else if(pid == 0){
		printf("pipe_process2\n");

		close(pipefds[1]);
		close(pipefds[2]);

		char pipe_write[20];
		char pipe_read[20];

		// convert to string
		snprintf(pipe_write, sizeof(pipe_write), "%d", pipefds[3]);
		snprintf(pipe_read, sizeof(pipe_read), "%d", pipefds[0]);

		execl("./pipe_p2.exe", "pipe_p2.exe", pipe_read, pipe_write, (char *)NULL);

	}
	else{
		printf("pipe_process1\n");

		close(pipefds[0]);
		close(pipefds[3]);

		char writemessages[2][20] = {"Hi", "Hello"};
		char readmessage[20];

		int write_val = 1;
		int read_val = 0;

		printf("Process1 write: %s\n", writemessages[0]);
		write(pipefds[1], writemessages[0], sizeof(writemessages[0]));

		read(pipefds[2], readmessage, sizeof(readmessage));
		printf("Process1 read: %s\n", readmessage);


		// integer
		printf("Process1 write: %d\n", write_val);
		write(pipefds[1], &write_val, sizeof(write_val));

		read(pipefds[2], &read_val, sizeof(read_val));
		printf("Process1 read: %d\n", read_val);


		// 寫了但沒人接,也可以往後走
		printf("Process1 write: %s\n", writemessages[0]);
		write(pipefds[1], writemessages[0], sizeof(writemessages[0]));
	}


	// execl("./pipe_process2.exe", "pipe_process2.exe", (char *)NULL);
	// execl("/bin/ls", "ls", "-l", (char *)NULL);
	// execlp("ls", "ls", "-a", (char *)NULL);
	

	printf("End of pipe_process1\n");

	return 0;
}