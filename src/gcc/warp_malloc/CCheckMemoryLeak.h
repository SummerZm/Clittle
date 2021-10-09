/*************************************************************************
 > File Name: CCheckMemoryLeak.h
 > Author: Leafxu
 > Created Time: Thu 18 Jul 2019 08:15:52 PM CST
 ************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
 
#ifdef __cplusplus
extern "C" {
#endif
    ///private
    void *__wrap_malloc(size_t size);
    void *__wrap_realloc(void *__ptr, size_t size);
    void *__wrap_calloc(size_t numElements, size_t sizeOfElement);
    void __wrap_free(void *ptr);
    ///pulibc
    void CCheckMemoryLeakInfo(void);
    void CCheckMemoryLeakSetDymSymbolDir(const char *dir);
    void CCheckMemoryLeakSetMallocSizeCheckPoint(size_t size);
    size_t CCheckMemoryLeakGetMallocSizeInfo(void);
#ifdef __cplusplus
}
#endif