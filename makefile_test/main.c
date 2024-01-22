#include <stdio.h>
#include <math.h>
#include "sum.h"

int main(){

	int x=1, y=2;
	int z;

	z = sum(x,y);
	printf("%d\n", z);

	double f1=0.5;
	double f2;
	f2 = ceil(f1);
	printf("%lf\n", f2);

	return 0;
}
