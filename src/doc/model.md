## 主题：C语言文件模块与作用域 ##

> ### Static关键词将变量的作用域限定在当前文件，进而实现模块化 ###
示例代码:
```c
// model1.c
#include<stdio.h>
static int zm = 1;
void printVal1()
{
	printf("Model 1: %d\n", zm++);
}

// model2.c 
#include<stdio.h>
static int zm = 2;
void printVal2()
{
	printf("Model 2: %d\n", zm++);
}

// main.c
#include<stdio.h>
int zm = 120;
void printVal1();
void printVal2();
int main(int argc, char** agrv) {
    int zm =20;
	printVal1();
	printVal2();
	printf("zm: %d\n", zm);
	return 0;
}
```
输出
```
Model 1: 1
Model 2: 2
zm: 20
```


> ### C语言的作用域不一定是就近原则 ###
代码示例:
```C
//修改 model1.c
#include<stdio.h>
int zm;
void printVal1()
{
	printf("Model 1: %d\n", zm++);
}
```
输出
```
Model 1: 120   [注：不是main函数中的zm, 而是全局变量的zm, 不就近原则]
Model 2: 2
zm: 20 [注: 就近原则]
```




