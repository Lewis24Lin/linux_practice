#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]){

	int i;

	// for(i=0;i<argc;i++){
	// 	printf("%s\n", argv[i]);
	// }

	int pipefds[2];
	pipefds[0] = atoi(argv[1]); // read
	pipefds[1] = atoi(argv[2]); // write

	char writemessages[2][20] = {"Hi from process2", "Hello from process2"};
	char readmessage[20];

	int write_val = 2;
	int read_val = 0;

	read(pipefds[0], readmessage, sizeof(readmessage));
	printf("Process2 read: %s\n", readmessage);

	printf("Process2 write: %s\n", writemessages[0]);
	write(pipefds[1], writemessages[0], sizeof(writemessages[0]));


	// integer
	read(pipefds[0], &read_val, sizeof(read_val));
	printf("Process2 read: %d\n", read_val);

	printf("Process2 write: %d\n", write_val);
	write(pipefds[1], &write_val, sizeof(write_val));

	printf("End of pipe_process2\n");

	return 0;
}