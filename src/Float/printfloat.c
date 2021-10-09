/*************************************************************************
 > File Name: printfloat.c
 > Author: Leafxu
 > Created Time: Thu 25 Jul 2019 11:30:23 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Float {
	/* 这个排序数顺序 和大小端有关*/
	unsigned int exp:23;
	unsigned int mant:8;
	unsigned int sign:1;
}Float;

int itob(int val, char* buff, int len, int base) {	
	int i=0, n=0;
	int tmp = val;
	char* tmpbuff = (char*)malloc(len);
	memset(buff, 0, len);
	memset(tmpbuff, 0, len);
	while(i<len && tmp>0) {
		sprintf(tmpbuff+i, "%d", tmp%base);
		//tmpbuff[i] = (char)(tmp%base); -- ascii 不可见字符
		tmp /=base;
		i++;
	}

	i = strlen(tmpbuff)-1;
	while(i>=0) 
		buff[n++] = tmpbuff[i--];

	return 1;
}

/* bit-feild 是不能取地址的*/
int itobit(int val, char* buff, int len) {
	int i = len-1;
	while(i>=0){
		if((val&(0x1<<i)) != 0) {
			buff[i] = '1';
		}
		else {
			buff[i] = '0';
		}
		i--;
	}
	return 1;
}


int main (int agrc, char** agrv) {
	char strbin[33] = {0};
	float F = 19.625;
	Float *f = (Float*)&F;
	printf("exp : %x %d\n", f->exp, f->exp);
	printf("mant: %x %d\n", f->mant, f->mant);
	printf("sign: %x %d\n", f->sign, f->sign);

	itob(f->exp, strbin,33, 2);
	printf("exp : %s\n", strbin);
	itob(f->mant, strbin, 33,2);
	printf("mant: %s\n",strbin);
	itob(f->sign, strbin, 33,2);
	printf("sign: %s\n", strbin);
	itobit(f->mant, strbin, 32);
	printf("mant: %s\n", strbin);
	itobit(f->exp, strbin, 32);
	//itobit(&(f->exp), strbin, 32);
	/* bit-feild 是不能取地址的*/
	printf("exp : %s\n", strbin);
	return 0;
}

