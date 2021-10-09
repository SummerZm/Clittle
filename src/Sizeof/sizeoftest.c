/*****************************************************************
 *  sizeof(expr) 并不计算expr
 *  url: https://blog.csdn.net/njnu_mjn/article/details/6897969
 * ***************************************************************/

#include <stdio.h>

struct Stu {
    char stuno[10];
    char name[32];
};

void func(int c[])
{
    printf("sizeof func array %lu\n", sizeof(c));
    return;
}

static int Num[3] = {12};
static char* name = "Hello leafxu";
static char* Msg[12] = {0};
int main() {   
    printf("%lu\n", (unsigned long)sizeof((struct Stu*)0)->stuno);  /* print 10 */

    int i = 2;
    printf("%lu\n", (unsigned long)sizeof(++i)); /* print 4 */
    printf("%d\n", i);                           /* print 2 (i isn't changed)*/
    int a = 5;
    int b = 3;
    int c[12] = {0};
    printf("%lu\n", (unsigned long)sizeof(a=b+1)); /* print 4 */
    printf("%d\n", a);                             /* print 5 (a isn't changed)*/

    // 32/64位机
    printf("char: %lu\n", sizeof(char));       /* print: 1 */
    printf("short: %lu\n", sizeof(short));      /* print: 2 */
    printf("int: %lu\n", sizeof(int));        /* print: 4 */
    printf("long: %lu\n", sizeof(long));       /* print: 4  8*/
    printf("float: %lu\n", sizeof(float));      /* print: 4 */
    printf("double: %lu\n", sizeof(double));     /* print: 8 */
    printf("long double: %lu\n", sizeof(long double));/* print: 8  16*/
    printf("char*: %lu\n", sizeof(char*));		/* char*: 8  16*/
    printf("int*: %lu\n", sizeof(int*));		/* int*: 8  16*/

    printf("sizeof array %lu\n", sizeof(c));
    func(c);
    
    printf("sizeof static int %lu\n", sizeof(Num)); /*print 12*/
    printf("sizeof static char* %lu\n", sizeof(name)); /*print 8*/
    printf("sizeof static char[]  %lu\n", sizeof(Msg)); /*print 96*/
    printf("sizeof'A' %lu\n", sizeof'A'); /*print 8*/
    return 0;
}
