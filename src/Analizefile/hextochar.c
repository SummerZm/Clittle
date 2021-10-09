#include <stdio.h>

/*
 假如  版本号是  17.17.17.17  
 固件接口：   0001 0001  0001 0001  0001 0001 0001 0001   【二进制】
                       17.17.17.17    【%d】
                       不可见字符      【%c】
 hex 文件：
          0011 0001  0011 0001    -   0011 0001 0011 0001    -   0011 0001    0011 0001     -  0011 0001   0011 0001 【二进制】 这里发生这 acsii 字符的转换
      
          转换过程
               0001          0001         -      0001           0001        -      0001                0001         -    0001               0001
 
           48 48 . 48 48. 48 48. 48 48    【%d】
           11.11.11.11    【%c】

 编辑器中显示     11  11 11  11
 我的代码取的是： 1,3,5,7位，丢了 0,2,4,6位<F12>
*/
int translateHexToChars(char* src, char* dst, int slen, int dlen)
{
	int ret = 0;
	if(src!=NULL && dst!=NULL && slen == dlen*2)
	{
		printf("translateHexToChars src: %s\n", src);
		unsigned char ch1, ch2;
		while(slen>0) {
			//*dst++ = (*src-48)<<4 | *++src-48;
			*dst++ = (*src-48)<<4 | (*++src-48);
			slen -= 2;
			src++;
		}
		ret = 1;
	}
	return ret;
}

int translateHexToChars1(char* src, char* dst, int slen, int dlen)
{
	int ret = 0;
	if(src!=NULL && dst!=NULL && slen == dlen*2)
	{
		printf("translateHexToChars src: %s\n", src);
		unsigned char ch1, ch2;
		while(slen>0) {
			*dst++ = *src<<4 | *++src;
			slen -= 2;
			src++;
		}
		ret = 1;
	}
	return ret;
}
