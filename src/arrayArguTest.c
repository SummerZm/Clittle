/*************************************************************************
 > File Name: test.c
 > Author: Leafxu
 > Created Time: Tue 02 Jul 2019 06:21:46 PM CST
 ************************************************************************/

#include<stdio.h>

typedef struct _r{
	int height;
	int width;
}R;

R info[] = {
	{10,10},
	{11,11}
};

void testfn2(R data[2] /*R** data  OK: R* data[] / R* data[2]*/) {
	// error: printf("height: %d\n", data->height); if argu is R** data
	// error: printf("height: %d\n", data[0]->height);
	//R* data1 = (R*)data;
	R* data1 = (R*)data;
	printf("height: %d\n", data1->height);
	data1->height = 113;
	printf("height: %d\n", data1->height);

	printf("height: %d\n", (data1+1)->height);
	//error: printf("height: %d\n", data1[1]->height);
	return;
}


int main() {
	//printf("height: %d\n", info[0].height);
	//testfn2(&info);
	testfn2(info);
	return 0;
}
