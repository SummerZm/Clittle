#include <stdio.h>
#include <stdlib.h>

void printfArray(int* arr, int len)
{
	if (arr!=NULL && len>0)
	{
		printf("[");
		for (int i=0; i<len; i++)
			printf("%d ", arr[i]);
		printf("]\n");
	}
}

void normal(int n, int m)
{
	int num = 0;
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<m; j++)
		{
			printf("%d\t", ++num);
		}
		printf("\n");
	}
}

/*
	1. [3sum]
 */
void twoPointer()
{
	int data[] = {1,2,3,4,5,6,7,8,9};
	int len = sizeof(data)/sizeof(int);
	printf("Input data: ");
	printfArray(data, 9);
	printf("Output data: \n");
	for (int i=0; i<len; i++) 
	{
		int cur = i;
		int l = cur+1;
		int r = len;
		printf("[%d] ", i);
		while (l<r) {
			printf("%d--><--%d ", l, r);
			l++;	
			r--;
		} 
		printf("\n");
	}
}

int main(int argc, char** argv)
{
	/* printf matix. */
	printf("Normal scan: \n");
	normal(10, 10);
	printf("\n");
	printf("3-sum scan:\n");
	twoPointer();
	printf("\n");
	return 0;
}











