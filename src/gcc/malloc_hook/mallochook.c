/*************************************************************************
 > File Name: mallochook.c
 > Author: Leafxu
 > Created Time: Fri 19 Jul 2019 04:14:30 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

typedef void *(*malloc_hook_fun_t)(size_t size, const void *caller);
typedef void  (*free_hook_fun_t)(void *ptr, const void *caller);

void replace_hook(void);
void restore_hook(void);

malloc_hook_fun_t old_malloc_hook = NULL;
free_hook_fun_t old_free_hook = NULL;

int replaced = 0;

void *my_malloc_fun(size_t size, const void *caller)
{
        restore_hook();
        void *ret = malloc(size);
        printf("+%p:[%p]\n", caller - 1, ret);
        replace_hook();
        return ret;
}

void my_free_fun(void *ptr, const void *caller)
{
        restore_hook();
        printf("-%p:[%p]\n", caller - 1, ptr);
        free(ptr);
        replace_hook();
}

void replace_hook(void)
{
        assert(!replaced);
        replaced = 1;
        old_malloc_hook = __malloc_hook;
        old_free_hook = __free_hook;
        __malloc_hook = my_malloc_fun;
        __free_hook = my_free_fun;

}

void restore_hook(void)
{
        assert(replaced);
        replaced = 0;
        __malloc_hook = old_malloc_hook;
        __free_hook = old_free_hook;
}

int main(int argc, char **argv)
{
        replace_hook();
        char *p = malloc(100);
        free(p);
        restore_hook();
        return 0;
}

