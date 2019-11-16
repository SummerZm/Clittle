#include <stdlib.h>
#include <stdio.h>

int main(int agrc, char** argv)
{
    char str[100];
    {
        char str1[100];
        printf("str1: %p\n", str1);
    }
    {
        char str2[100];
        printf("str1: %p\n", str2);
    }
    return 0;
}
