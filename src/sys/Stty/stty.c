/*************************************************************************
 > File Name: stty1.c
 > Author: Leafxu
 > Created Time: Sat 21 Apr 2018 03:02:19 PM CST
 > Desc: 实现字符输入 (linux下的输入单位一般是行.即:回车)
 >  每输入一个字符, 函数循坏处理
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

int main (int argc, char **agrv)
{
    int c;
    /* 将终端设置为 字符输入模式 */
    system("stty raw");

    while ( c!='q' ) {
        c = getchar();
        printf("%c", c);
    }

    /* 回到终端原来的 行输入模式 */
    system("stty cooked");
    return 0;
}
