#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 4

// multithread
// https://www.geeksforgeeks.org/multithreading-in-c/

// mutex
// https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/

// pthread_exit()
// https://shengyu7697.github.io/cpp-pthread_exit/
// http://c.biancheng.net/view/8608.html

// pthread_exit(), pthread_join()
// https://blog.csdn.net/lanzhihui_10086/article/details/40891299

// gcc thread_basic.c -lpthread

struct args{
	unsigned long *thread_id;
	int num;
	char text[64];
};

int global_var1 = 1;

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void *myThread(void *arg)
{
	sleep(1);
	printf("in thread\n");

	struct args *inputs = (struct args *)arg;
	printf("thread_id: %lu\n", *inputs->thread_id);
	printf("thread_id: %lu\n", pthread_self());
	printf("num: %d\n", inputs->num);
	printf("text: %s\n", inputs->text);

	static int static_local_var1 = 1;
	int local_var1 = 1;

	global_var1++;
	static_local_var1++;
	local_var1++;

	printf("global_var1: %d, static_local_var1: %d, local_val1: %d\n", global_var1, static_local_var1, local_var1);

	// pthread_exit() 會退出thread,裡面是回傳值,pthread_join()會收到這個值,沒有回傳就寫null
	// 後面不會再執行
 	pthread_exit((void *)"exit thread!");

 	// 在thread中,用return也可以退出thread
	// return (void *)"exit thread!";
}

int main()
{
	printf("Start\n\n");

	// pthread_self()可以拿到自己的therad_id
	printf("thread_id: %lu\n", pthread_self());
	printf("\n");

	int i;

	pthread_t thread_id[THREAD_NUM];

	struct args inputs[THREAD_NUM];

	int ret;
	void *ret_text;

	for(i=0;i<THREAD_NUM;i++){
		inputs[i].thread_id = &thread_id[i];
		inputs[i].num = i;
		// strcpy(inputs[i].text, "inputs");
		sprintf(inputs[i].text, "inputs%d", i);

		// param
		// 1. The first argument is a pointer to thread_id which is set by this function. 
		//    會寫值進去,可以用來辨識thread
		// 2. The second argument specifies attributes. If the value is NULL, then default attributes shall be used.
		// 3. The third argument is name of function to be executed for the thread to be created.
		// 4. The fourth argument is used to pass arguments to the function, myThreadFun.
		//    以指標的方式傳參數
		ret = pthread_create(&thread_id[i], NULL, myThread, &inputs[i]);

		if(ret!=0){
			printf("create thread fail");
			return 1;
		}

		// The pthread_join() function for threads is the equivalent of wait() for processes. 
		// A call to pthread_join blocks the calling thread until the thread with identifier equal to the first argument terminates.
		// pthread_exit()回傳的東西會放在第二個參數指定的位置,沒有收東西就寫null
		// pthread_join()自己會回傳狀態
		// 現在這樣一個等一個,創出來的thread_id好像都一樣
		ret = pthread_join(thread_id[i], &ret_text);
		if(ret!=0){
			printf("wait thread fail");
			return 1;
		}
		printf("%s\n", (char *)ret_text);
		printf("\n");
	}

	// pthread_join放在這邊,create不會等別的thread結束
	// thread之間也是交互執行？
	// for(i=0;i<THREAD_NUM;i++){
	// 	ret = pthread_join(thread_id[i], &ret_text);
	// 	if(ret!=0){
	// 		printf("wait thread fail");
	// 		return 0;
	// 	}
	// 	printf("%s\n", (char *)ret_text);
	// }
	

	printf("End\n");

	// 在main中用pthread_exit()只會退出當前的thread(main thread?),不會影響其他thread
	// 可以等其它thread都做完才全部結束
	// 不會再執行後面的東西
	pthread_exit(NULL);
	printf("after pthread_exit()\n");

	// 在main中用return會全部退出?
	return 0;
}
