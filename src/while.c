#include <stdio.h>
#include <stdlib.h>

void printfArray(int* arr, int len)
{
	if (arr!=NULL && len>0)
	{
		printf("[ ");
		for (int i=0; i<len; i++)
			printf("%d ", arr[i]);
		printf(" ]\n");
	}
}

void printfMatrix(int** arr, int n, int m)
{
	if (arr!=NULL &&  n>0 && m>0)
	{
		int (*_arr)[3] = (int*)arr;
		for (int i=0; i<n; i++) 
		{	
			printf("[ ");
			for (int j=0; j<m; j++) 
			{
				printf("%d ", _arr[i][j]);
			}
			printf("]\n");
		}
		printf("\n");
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
	printf("In: ");
	printfArray(data, 9);
	printf("Out: \n");
	for (int i=0; i<len; i++) 
	{
		int cur = i;
		int l = cur+1;
		int r = len-1;
		printf("[%d] ", i);
		while (l<r) {
			printf("%d--><--%d ", data[l], data[r]);
			l++;	
			r--;
		} 
		printf("\n");
	}
}

/* 
	Matrix point print. 
	Mod: should be 0/1
*/
void MatrixPointPrint(int mod)
{
	int cnt=0;
	int data[3][3] = {{1,3,5},{2,4,6},{7,8,9}};
	printfMatrix(data,3,3);
	for (int i=0; i<3; i++)
	{
		printf("[ ");
		for (int j=0; j<3; j++)
		{
			cnt++;
			if (cnt%2==mod)
				printf("%d ", data[i][j]);
			else
				printf("  ", data[i][j]);
		}
		printf("]\n");
	}
}

void MatrixPointPrint1(int mod)
{
	int cnt=0;
	int data[5][5] = {{1,3,5,7,8},{2,4,6,0,4},{1,2,3,4,5},{7,8,9,1,3},{9,8,7,6,5}};
	printfMatrix(data,5,5);
	for (int i=0; i<5; i++)
	{
		printf("[ ");
		for (int j=0; j<5; j++)
		{
			if (j%2==mod)
				printf("%d ", data[i][j]);
			else
				printf("  ", data[i][j]);
		}
		printf("]\n");
	}
}

void MatrixPointPrint2(int mod)
{
	int cnt=0;
	int data[3][3] = {{1,3,5},{2,4,6},{7,8,9}};
	printfMatrix(data,3,3);
	for (int i=0; i<3; i++)
	{
		if (i%2==mod) 
		{
			printf("[ ");
			for (int j=0; j<3; j++) 
			{
				printf("  ");
			}
			printf("]\n");
			continue;
		}
		printf("[ ");
		for (int j=0; j<3; j++)
		{
			printf("%d ", data[i][j]);
		}
		printf("]\n");
	}
}

void MatrixPointPrint3()
{
	int data[8][8] = {
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1}
	};
	printfMatrix(data, 8, 8);
	for (int i=0; i<8; i++)
	{
		printf("[ ");
		for (int j=0; j<8; j++)
		{
			if (i%2==0&& j%2==0)
			{
				printf("%d ", data[i][j]);
			}
			else
			{
				printf("- ", data[i][j]);
			}
		}
		printf("]\n");
	}
	
}

void MatrixPointPrint4()
{
	int data[8][8] = {
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1}
	};
	printfMatrix(data, 8, 8);
	for (int i=0; i<8; i++) 
	{
		printf("[ ");
		for (int j=0; j<8; j++)
		{
			if (i%2==1 && j%2==1)
			{
				printf("%d ", data[i][j]);
			}
			else
			{
				printf("- ");
			}
		}
		printf(" ]\n");
	}
}

void printfDiagonal()
{
	int data[8][8] = {
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1}
	};
	printfMatrix(data, 8, 8);
	int len = 8;
	for (int i=0; i<len; i++) 
	{
		printf("[ ");
		for (int j=0; j<len; j++)
		{
			if (i-j==0 || j==len-1-i)
			{
				printf("%d ", data[i][j]);
			}
			else
			{
				printf("- ");
			}
		}
		printf(" ]\n");
	}
}

void ArrayRotate()
{
	int mod = 15;
	int data[] = {1,2,3,4,5,6,7,8,9};
	printf("\n>>>>>> ArrayRotate mod[6] <<<<<<<\n");
	printf("旋转数组\n");
	printfArray(data, 9);

	int len  = 9;
	int count = 0;
	mod %= len;
	int history[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
	for (int i=0; count<9; i++)
	{
		int prev = data[i];
		int curr = i;
		do 
		{
			int next = (curr+mod)%len;
			int tmp = data[next];
			data[next] = prev;
			prev = tmp;
			curr = next;
			count++;

			/**/
			history[curr] = 1;
			
			printf("[ ");
			for (int k=0; k<len; k++)
			{
				/*	
				// Display 1
				if (k==curr)
				{
					printf(" %d ", data[k]);
				}
				else
				{
					printf(" - ");
				}
				*/
				/*  
				// Display 2
				if (history[k]==1) {
					printf("<%d>", data[k]);
				}
				else 
				{
					printf(" %d ", data[k]);
				}
				*/
				// Display 3
				if (history[k]==1) {
					printf("%d ", data[k]);
				}
				else 
				{
					printf("- ");
				}
			}
			printf(" ] pos[%d] temp[%d] loop[%d]\n",curr, prev, count);

		}while(i!=curr);
		// printfArray(data, 9);
	}
	printfArray(data, 9);
}

void reverse(int* arr, int s, int e, int len) {
    if (e>len) return;
    int m = (s + e) / 2;    // Get mid value
    for (int i=s; i<=m; i++) {
        int temp = arr[i];
        arr[i] = arr[e-i+s-1];
        arr[e-i+s-1] = temp;
		printfArray(arr, 10);
    }
    return;
}

int main(int argc, char** argv)
{
	/* printf matix. */
	printf(">>>> Normal scan <<<<\n");
	printf("普通双层循环打印:\n");
	normal(10, 10);
	printf("\n");
	printf(">>>> 3-sum scan <<<<\n");
	printf("排好序解决3数问题:\n");
	twoPointer();
	printf("\n");
	printf(">>>> Matrix point print <<<<\n");
	printf("打印矩阵中的偶数个数\n");
	MatrixPointPrint(1);
	printf("\n\n");
	printf("打印矩阵中的每一行的第偶个数\n");
	MatrixPointPrint1(0);
	printf("\n\n");
	printf("\n打印矩阵中的第偶数行\n");
	MatrixPointPrint2(0);
	printf("\n打印矩阵左上值\n");
	MatrixPointPrint3();
	printf("\n打印矩阵右下值\n");
	MatrixPointPrint4();
	printf("\n打印矩阵对角线\n");
	printfDiagonal();
	ArrayRotate();

	printf("\n>>>>> Reverse arr <<<<<\n");
	printf("翻转指定区域数组\n");
	int data[] = {1,2,3,4,5,6,7,8,9, 10};
	printfArray(data, 10);
	reverse(data, 0, 9, 10);
	printfArray(data, 10);
	return 0;
}











