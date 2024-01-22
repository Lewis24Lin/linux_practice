#include <stdio.h>

void func1(int *p1, int *p2){
	// 只交換了p1,p2的值 並沒有交換到a,b的值
	int *p = p1;
	p1 = p2;
	p2 = p;
}

int func2(int x, int y){
	return x+y;
}

int func3(){
	int arr[128]={};
	char i;
	for(i=0;i<128;i++){
		arr[i] = 1;
	}
	printf("%d\n", arr[0]);
}

int main(void){

	int a = 3, b = 4;
	int c = a-b;
	printf("%d %d %d\n", a, b, c);

	func1(&a, &b);

	c = func2(a,b);

	int i;
	int sum = 0;
	for(i=0;i<=10;i++){
		sum += i;
	}

	func3();
	
	return 0;
}