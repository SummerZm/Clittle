/*************************************************************************
 > File Name: hackcontrol.c
 > Author: Leafxu
 > Created Time: Tue 23 Jul 2019 10:22:47 AM CST
 ************************************************************************/
#include <stdarg.h>
#include <dlfcn.h>
//#define _GNU_SOURCE 
#define NULL ((void*)0)

static int (*_printf)(const char *format, ...) = NULL;
int printf(const char *format, ...)
{
	if (_printf == NULL) {
		/* 取得标准库中的printf的函数地址 */
		_printf = (int (*)(const char *format, ...)) dlsym(RTLD_NEXT, "printf");
		/* 把函数返回的地址置到<main+75>  根据具体汇编跳转设置, gdb disassemble main*/
		__asm__ __volatile__ (
                "movl 0x4(%ebp), %eax \n\t"
                "addl $15, %eax \n\t"
                "movl %eax, 0x4(%ebp)"
         );
		return 1;
	}
	/* 重置 printf的返回地址 */
	 __asm__ __volatile__ (
		"addl $12, %%esp \n\t"
            "jmp *%0 \n\t"
           :/* no output registers */
           : "g" (_printf)
           : "%esp"
	);
}

