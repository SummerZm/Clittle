/*************************************************************************
 > File Name: stot.c
 > Author: Leafxu
 > Created Time: Tue 09 Jul 2019 04:17:31 PM CST
 ************************************************************************/

#include<stdio.h>
#include <iconv.h>
#include <opencc/opencc.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

// 链接时使用 -lopencc

//使用iconv库将字符串从编码from_charset转换到to_charset，输入是inbuf，输出是outbuf
void m_iconv(const char *from_charset, const char * to_charset,
	const char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	memset(outbuf,0,outlen);
	iconv_t cd;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return;
	iconv(cd, (char **)&inbuf, &inlen, &outbuf, &outlen);
	iconv_close(cd);
}

///繁体转化为简体，编码从src_charset转换为dest_charset，输入是src，输出是dest
void cht2chs(const char* src_charset, const char * dest_charset, const char *src, char *dest) {
	char utf8[1000], tempout[1000], *raw=(char *)src;
	m_iconv(src_charset, "utf8", raw, strlen(raw), utf8, sizeof(utf8)); //先转为utf8
	opencc_t od = opencc_open(OPENCC_DEFAULT_CONFIG_TRAD_TO_SIMP);
	char * chsutf8 = opencc_convert_utf8(od, utf8, strlen(utf8)); //简繁转换
	strcpy(tempout, chsutf8);
	free(chsutf8); //一定要free不然会内存泄漏
	opencc_close(od);
	size_t len = strlen(tempout);
	m_iconv("utf8", dest_charset, tempout, len, dest, len*2+1); //再转到目标编码
}

///简体转化为繁体，编码从src_charset转换为dest_charset，输入是src，输出是dest
void chs2cht(const char* src_charset, const char * dest_charset, const char *src, char *dest) {
	char utf8[1000], tempout[1000], *raw=(char *)src;
	m_iconv(src_charset, "utf8", raw, strlen(raw), utf8, sizeof(utf8)); //先转为utf8
	opencc_t od = opencc_open(OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD);
	char * chsutf8 = opencc_convert_utf8(od, utf8, strlen(utf8)); //简繁转换
	strcpy(tempout, chsutf8);
	free(chsutf8); //一定要free不然会内存泄漏
	opencc_close(od);
	size_t len = strlen(tempout);
	m_iconv("utf8", dest_charset, tempout, len, dest, len*2+1); //再转到目标编码
}

int main() //main里放测试代码
{
	/*char src[] = "\264\372\270\325", dest[100]; //big5编码的繁体的"测试"
	printf("%s\n", src); //输出"测试"
	cht2chs("big5", "utf8", src, dest); //根据实际情况，把"utf8"改为你的locale
	printf("%s\n", dest); //输出"测试"*/
	char src[] = "分机", dest[100];
	printf("%s\n", src); //输出"测试"
	chs2cht("utf8", "big5", src, dest);
	printf("%s\n", dest); //输出"测试"
	return 0;
}

