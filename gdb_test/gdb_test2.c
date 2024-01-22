#include <stdio.h>
#include <stdlib.h>

#define max(a,b) ((a)>=(b)?(a):(b))

typedef struct node{
	int id;
	struct node *left;
	struct node *right;
} Node;

//     n1
//   n2  n3
// n4


int getDepth(Node *root){
	if(!root) return 0;

	int left_d = getDepth(root->left);
	int right_d = getDepth(root->right);
	int depth = max(left_d, right_d)+1;
	return depth;
}

int main(void){

	Node *n1 = (Node *)calloc(1, sizeof(Node));
	n1->id = 1;

	Node *n2 = (Node *)calloc(1, sizeof(Node));
	n2->id = 2;

	Node *n3 = (Node *)calloc(1, sizeof(Node));
	n3->id = 3;

	Node *n4 = (Node *)calloc(1, sizeof(Node));
	n4->id = 4;

	n1->left = n2;
	n1->right = n3;
	n2->left = n4;

	int depth = getDepth(n1);
	printf("%d\n", depth);

	return 0;
}
