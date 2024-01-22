#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// https://www.geeksforgeeks.org/fork-system-call/

int main(){

	// x在parent跟child的physical address是不同的, 可是virtual address是相同的

	int x = 1;

	if(fork() == 0){
		// printf("child, x = %d\n", ++x);
		// scanf("%d"	, &x);
		printf("child, x = %d\n", x);
		printf("child, &x = %p\n", &x);
	}
	else{
		printf("parent, x = %d\n", x++);
		printf("parent, x = %d\n", x);
		printf("parent, &x = %p\n", &x);
	}

	return 0;
}