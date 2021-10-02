/*************************************************************************
 > File Name: control.c
 > Author: Leafxu
 > Created Time: Tue 23 Jul 2019 10:19:20 AM CST
 ************************************************************************/

#include<stdio.h>
int main(int argc, char **argv)
{
	int a = 1, b = 2;
	if (a != b) {
		printf("Sorry!\n");
		return 0;
	}
	printf("OK!\n");
	return 1;
}
