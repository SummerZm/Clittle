/*************************************************************************
 > File Name: TestChkMemLeak.cpp
 > Author: Leafxu
 > Created Time: Thu 18 Jul 2019 08:16:52 PM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>
 
#include "CCheckMemoryLeak.h"
 
pthread_t monitor_id = -1;
int thread_exit = 0;
pthread_t monitor_id2 = -1;
int thread_exit2 = 0;
 
void *monitor(void *args)
{
    int i = 0;
    while (0 == thread_exit)
    {
        if (10 > i)
        {
            char *p = (char *)malloc(10);
            printf("(%p)thread 1 exit(%d)\n", p, i);
            free(p);
        }
        else{
            printf("thread 1 exit(%d)\n", i);
            break;
        }
        ++i;
    }
 
    return NULL;
}
 
void *monitor2(void *args)
{
    int i = 0;
    while (0 == thread_exit2)
    {
        if (10 > i)
        {
            char *p = (char *)malloc(10);
            printf("(%p)thread 2 exit(%d)\n", p, i);
            free(p);
        }
        else{
            printf("thread 2 exit(%d)\n", i);
            break;
        }
        i++;
    }
 
    return NULL;
}
 
static void malloc_size_monitor_init(void)
{
    if (0 != pthread_create(&monitor_id, NULL, monitor, NULL))
    {
        monitor_id = -1;
    }
}
 
static void malloc_size_monitor_exit(void)
{
    thread_exit = 1;
    if (-1 != monitor_id)
    {
        pthread_join(monitor_id, NULL);
    }
}
 
static void malloc_size_monitor_init2(void)
{
    if (0 != pthread_create(&monitor_id2, NULL, monitor2, NULL))
    {
        monitor_id2 = -1;
    }
}
 
static void malloc_size_monitor_exit2(void)
{
    thread_exit2 = 1;
    if (-1 != monitor_id2)
    {
        pthread_join(monitor_id2, NULL);
    }
}
 
void next()
{
    printf("\npause now, press 'n' to contine\n");
    char c = 'x';
    while (1)
    {
        scanf("%c", &c);
        if ('n' == c)
        {
            return;
        }
    }
}
int main(int argc, char *argv[])
{
    int len = 100;
    char *p = (char *)malloc(len);
    if (NULL != p)
    {
        memset(p, 0x1F, len);
        CCheckMemoryLeakInfo();
        char *p2 = (char *)realloc(p, 2000);
        CCheckMemoryLeakInfo();
        free(p2);
        CCheckMemoryLeakInfo();
        p = NULL;
    }
 
    p = (char *)calloc(2, len);
    free(p);
    p = (char *)calloc(2, 200);
    free(p);
#if 1
    malloc_size_monitor_init();
    malloc_size_monitor_init2();
    malloc_size_monitor_exit();
    malloc_size_monitor_exit2();
    next();
#endif
    return 0;
}
