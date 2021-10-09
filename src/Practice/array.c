#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size1 5
#define size2 10
#define size3 15
int main()
{
	int ***arr;
	int i,j,k;
	arr = (int***)malloc(sizeof(int**)*size1);
	if (arr==NULL)  printf("arr leve11 malloc fialed.\n");
	for (i=0; i<size1; i++) {
		arr[i] = (int**)malloc(sizeof(int*)*size2);
		if (arr[i]==NULL)  printf("arr leve12 malloc fialed.\n");
		for (j=0; j<size2; j++) {
			if (arr[i][j]==NULL)  printf("arr leve13 malloc fialed.\n");
			arr[i][j] = (int*)malloc(sizeof(int)*size3);
		}
	}
	
	for (i=0; i<size1; i++) {
		for (j=0; j<size2; j++) {
			for (k=0; k<size3; k++) {
				arr[i][j][k] = i*j*k;
			}
		}
	}

	for (i=0; i<size1; i++) {
		for (j=0; j<size2; j++) {
			for (k=0; k<size3; k++) {
				printf("i*j*k = %d*%d*%d = %d\n", i, j, k, arr[i][j][k]);
			}
		}
	}

	for (i = 0; i < size1; i ++) {
		for (j = 0; j < size2; j ++) {
			memset(arr[i][j], 0, sizeof(int)*size3);
		}
	}

	for (i = 0; i < size1; i ++) {
		for (j = 0; j < size2; j ++) {
			free(arr[i][j]);
		}
		free(arr[i]);
	}
	free(arr);
	return 0;
}
