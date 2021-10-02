/*************************************************************************
 > File Name: pwdtest.c
 > Author: Leafxu
 > Created Time: Tue 23 Jul 2019 09:30:10 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
int main(int argc, char **argv){
	char passwd[] = "password";
	if (argc < 2) {
		printf("usage: %s <password>\n", argv[0]);
		return;
	}
	if (!strcmp(passwd, argv[1])) {
		printf("Correct Password!\n");
		return;
	}
	printf("Invalid Password!\n");
}
//https://blog.csdn.net/haoel/article/details/1602108 
