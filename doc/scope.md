## 主题 C语言中代码块 {...} 中声明的变量在离开代码块会被回收 ##
示例代码：
```C
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
        printf("str2: %p\n", str2);
    }
    return 0;
}
```
输出结果：
```
str1: 0xffffcb40
str2: 0xffffcb40
```
地址打印一致，说明str1所在栈空间被回收，并重新分配给str2.

## 隐患代码 ##
```C
#include <stdlib.h>
#include <stdio.h>

void startThread(const char* filename) 
{
    //Ok: 将数据拷贝到堆中, 传入另外一个线程。
    //Error: 不拷贝数据，直接将filename(栈空间地址), 传入另外一个线程。
}

int main(int agrc, char** argv)
{
    // Init ...
    {
        char *path = "/Home/Hello";
        char file[100];
        strcpy(file, path);
        startThread(file);
    }
    // Do other thing ...
    return 0;
}
```
## 代码作用域 - 栈地址 - 野指针 ##
说明：将栈空间地址传入其他线程或保存至某个全局或静态变量，将会导致野指针, 极度危险.  
原因：在作用域外持有栈空间中的指针/地址，其数据得不到保障，可能被自动分配或回收.  
表现：段错误，数据误改，偶然性幽灵Bug.  