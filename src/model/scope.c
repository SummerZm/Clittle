#include <stdlib.h>
#include <stdio.h>

int main(int agrc, char** argv)
{
    char str[100];
    printf("str: %p\n", str);
    strcpy(str, "Test1");
    printf("str: %p %s\n", str, str);
    {
        char str1[100];
        printf("str1: %p\n", str1);
        strcpy(str1, "Test1");
        printf("str1: %p %s\n", str1, str1);
    }
    {
        // 上一个代码块被会回收,栈地址空间被重复利用 
        char str2[100];
        printf("str2: %p\n", str2);
        strcpy(str2, "Test2");
        printf("str2: %p %s\n", str2, str2);
    }
    return 0;
}
