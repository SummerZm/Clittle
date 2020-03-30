/*************************************************************************
 > File Name: encript.c
 > Author: Leafxu
 > Created Time: 2020年03月30日 星期一 20时29分36秒
 ************************************************************************/

#include<stdio.h>

int encrypt(const char* data, const char* slat, char* buffer, int len)
{
	int ret = 0;
	if (data!=NULL && slat!=NULL && buffer!=NULL) 
	{	
		int dLen = strlen(data);
		int sLen = strlen(slat);
		int bLen = sizeof(buffer);
		if (bLen>=dLen) 
		{
			memset(buffer, 0, len);
			for (int i=0; i<dLen; i++)
			{
				printf("%c", slat[i%sLen]);
				buffer[i] = data[i]^slat[i%sLen];
			}
			ret = 1;
			printf("\n");
		}
	}
	return ret;
}

int main()
{
	char* data= "ABCDEFG";
	char* slat= "jhk";
	char buffer1[10];
	char buffer2[10];
	printf("result:%d\n", encrypt(data, slat, buffer1, sizeof(buffer1)));
	printf("buffer1:%s\n", buffer1);
	printf("result:%d\n", encrypt(buffer1, slat, buffer2, sizeof(buffer2)));
	printf("buffer2:%s\n", buffer2);
	return 0;
}

