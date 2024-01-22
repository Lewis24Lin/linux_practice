#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// fork
// https://www.geeksforgeeks.org/fork-system-call/

int main(){
	

	// fork();
	// 為什麼cmd中, 兩個以上fork在執行的時候目錄會在中間出現？ 而且都出現在第四個位置？
	// 然後後面的output看起來變成後續的指令
	// https://stackoverflow.com/questions/50454678/terminal-output-incorrect-for-a-program-using-fork
	// 因為parent沒等child, 自己先做完了？ 為什麼我是parent先做？ 看一些範例都是child先？ 且為什麼不會concurrency?
	// fork();
	// fork();
	// fork();

	// fork();
	// if(fork()==0){
	// 	printf("child process %d\n", getpid());
	// }
	// printf("Hello World from process %d\n", getpid());


	int A;
	A = fork();

	if(A == 0){
		printf("child %d\n", getpid());
	}
	else{
		printf("parent %d\n", getpid());
		int status;
		int child_pid = wait(&status); // pass an int pointer to wait() to store the child's exit status
		printf("parent %d, child %d complete\n", getpid(), child_pid);
	}

	
	int B;
	B = fork();
	if(B == 0){
		printf("child 1 %d\n", getpid());
	}
	else{
		printf("parent 1 %d\n", getpid());
		int status;
		int child_pid = wait(&status); // pass an int pointer to wait() to store the child's exit status
		printf("parent 1 %d, child %d complete\n", getpid(), child_pid);
	}

	printf("process %d, end\n", getpid());

	return 0;
}