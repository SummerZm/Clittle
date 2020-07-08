/*************************************************************************
 > File Name: checkip.c
 > Author: Leafxu
 > Created Time: 2020年07月08日 星期三 12时31分22秒
 ************************************************************************/

#include<stdio.h>

int check_ip_format(const char* ipaddress)
{
	if(ipaddress != NULL){
		int a, b, c, d;
		const char* format = "%d.%d.%d.%d";	
		if(sscanf(ipaddress, format, &a, &b, &c, &d)==4 && a>=0 && a<=255 &&
			b>=0 && b<=255 && c>=0 && c<=255 && d>=0 && d<=255){
			return 1;
		}
	}
	return 0;
}
