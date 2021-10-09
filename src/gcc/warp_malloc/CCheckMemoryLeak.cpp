/*************************************************************************
 > File Name: CCheckMemoryLeak.cpp
 > Author: Leafxu
 > Created Time: Thu 18 Jul 2019 08:13:13 PM CST
 ************************************************************************/
#include <stdlib.h>
#include <execinfo.h>
#include <map>
#include <utility>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <cxxabi.h>
#include <dirent.h>
 
#include "CCheckMemoryLeak.h"
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// declare
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define THREAD_AUTOLOCK automutex _lock_(&_mutex, __FUNCTION__, __LINE__);
#define TRUE (1)
#define FALSE (0)
#define PERR printf
#define PWRN printf
 
#define __HOOK__ (0)
 
class automutex
{
public:
	automutex(pthread_mutex_t *mux, const char *func, const int line);
	~automutex();
 
private:
    pthread_mutex_t *m_mux;
};
 
automutex::automutex(pthread_mutex_t *mux, const char *func, const int line)
{
	m_mux = mux;
	pthread_mutex_lock(m_mux);
}
 
automutex::~automutex()
{
	pthread_mutex_unlock(m_mux);
}
 
typedef struct BtInfo_s
{
    size_t size;
    char **str_bt;
    size_t mem_size;
    pthread_t tid;
    pid_t pid;
} BtInfo_t;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// internel params
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void (*old_free)(void *ptr, const void *caller);
static void *(*old_malloc)(size_t size, const void *caller);
static void *(*old_realloc)(void *ptr, size_t size, const void *caller);
static void *(*old_memalign)(size_t alignment, size_t size, const void *caller);
 
static void my_free(void *ptr, const void *caller);
static void *my_malloc(size_t size, const void *caller);
static void *my_realloc(void *ptr, size_t size, const void *caller);
static void *my_memalign(size_t alignment, size_t size, const void *caller);
 
static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
static std::map<const void *,  BtInfo_t> _malloc_info_map;
static char _dym_symbol_dir[256] = "";
static size_t _malloc_size_check_point = 1024;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// internel function
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int get_backtrace_info(char ** &str_bt, size_t *bt_sz)
{
    if (NULL == bt_sz)
    {
        PERR("[%s][%d] error bt is null\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
#define _BT_SIZE_ (10)
    void *array[_BT_SIZE_] = { NULL };
    size_t size = backtrace (array, _BT_SIZE_);
#undef _BT_SIZE_
    if (0 == size)
    {
        PERR("[%s][%d] bt sz is 0\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
 
    str_bt = backtrace_symbols (array, size);
    if (NULL == str_bt)
    {
        PERR("[%s][%d] error str_bt is null\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
 
    *bt_sz = size;
    return TRUE;
}
 
static pid_t gettid(void)
{
    return syscall(SYS_gettid);
}
 
static int save_malloc_pointer_info(const void *ptr, const size_t size)
{
    if (NULL == ptr)
    {
        PWRN("ptr is null\n");
        return TRUE;
    }
 
    size_t bt_sz = 0;
    BtInfo_t stBtInfo;
    if (FALSE == get_backtrace_info(stBtInfo.str_bt, &bt_sz))
    {
        PWRN("[%s][%d]get bt info error\n", __FUNCTION__, __LINE__);
        stBtInfo.str_bt = NULL;
    }
 
    stBtInfo.size = bt_sz;
    stBtInfo.mem_size = size;
    stBtInfo.tid = pthread_self();
    stBtInfo.pid = gettid();
    _malloc_info_map.insert(std::make_pair<const void *,  BtInfo_t>(ptr, stBtInfo));
    return TRUE;
}
 
static int remove_free_pointer_info(const void *ptr)
{
    if (NULL == ptr)
    {
        PWRN("[%s][%d]ptr is null\n", __FUNCTION__, __LINE__);
        return TRUE;
    }
    std::map<const void *,  BtInfo_t>::iterator _it;
    _it = _malloc_info_map.find(ptr);
    if (_it == _malloc_info_map.end())
    {
        PERR("[%s][%d]point(%p) not found, must somethine error\n", __FUNCTION__, __LINE__, ptr);
        return FALSE;
    }
 
    BtInfo_t *pstBtInfo = &(_it->second);
    if (NULL != pstBtInfo)
    {
        if (NULL != pstBtInfo->str_bt)
        {
            free(pstBtInfo->str_bt);
            pstBtInfo->str_bt = NULL;
        }
    }
 
    _malloc_info_map.erase(_it);
    return TRUE;
}
 
/// format string start
static int _frame_pos = 0;
static void format_output_info(const char *funcname, const char *file, int line)
{
    if (NULL == funcname)
    {
        printf("#%d  (Unkonw Function Name) at (Unknow Source File):0\n", _frame_pos);
    }
    else if (NULL == file)
    {
        printf("#%d  %s() at (Unknow Source File):0\n", _frame_pos, funcname);
    }
    else
    {
        printf("#%d  %s() at %s:%d\n", _frame_pos, funcname, file, line);
    }
    _frame_pos++;
}
 
static int get_libname(const char *symbol, char *libname)
{
    if ((NULL == symbol) || (NULL == libname))
    {
        return FALSE;
    }
 
    const char *end = strstr(symbol, ".so");
    if (NULL == end)
    {
        return FALSE;
    }
 
    char *sta = (char *)rindex(symbol, '/');
    int len = 0;
    if (NULL == sta)
    {
        sta = (char *)symbol;
    }
    else
    {
        sta = sta + 1;
    }
 
    len = end + 3 - sta;
    if (0 >= len)
    {
        return FALSE;
    }
 
    strncpy(libname, sta, len);
    return TRUE;
}
 
static int get_funcname(const char *symbol, char *funcname)
{
    if ((NULL == symbol) || (NULL == funcname))
    {
        return FALSE;
    }
 
    const char *sta = strchr(symbol, '(');
    if (NULL == sta)
    {
        return FALSE;
    }
 
    const char *end = strchr(symbol, '+');
    if (NULL == end)
    {
        return FALSE;
    }
 
    sta = sta + 1;
    int len = end - sta;
    if (1 >= len)
    {
        return FALSE;
    }
 
    strncpy(funcname, sta, len);
    return TRUE;
}
 
static int trans_symbol_name(char *symbol_name)
{
    if (NULL == symbol_name)
    {
        return FALSE;
    }
 
    int status = 0;
    size_t size = 0;
    char *realname = abi::__cxa_demangle(symbol_name, 0, &size, &status);
    if (NULL != realname)
    {
        memset(symbol_name, 0x0, 256);
        strncpy(symbol_name, realname, 256);
        free(realname);
        realname = NULL;
        return TRUE;
    }
    return FALSE;
}
 
static void parse_func_info(const char *symbol)
{
    if (NULL == symbol)
    {
        return;
    }
 
    char funcname[256];
    memset(funcname, 0x0, sizeof(funcname));
    if (TRUE == get_funcname(symbol, funcname))
    {
        trans_symbol_name(funcname);
        format_output_info(funcname, NULL, 0);
    }
    else
    {
        format_output_info(symbol, NULL, 0);
    }
}
 
static int get_lib_symbol_path(const char *libname, char *libpath)
{
    if ((NULL == libname) || (NULL == libpath))
    {
        return FALSE;
    }
 
    struct dirent    *dp;
    DIR              *dfd = NULL;
 
    if(NULL == (dfd = opendir(_dym_symbol_dir)))
    {
        printf("open root dir failed! dir: %s", _dym_symbol_dir);
        return FALSE;
    }
 
    for(dp = readdir(dfd); NULL != dp; dp = readdir(dfd))
    {
        if(strstr(dp->d_name, libname) != NULL)
        {
            strncpy(libpath, dp->d_name, 256);
            closedir(dfd);
            return TRUE;
        }
    }
 
    closedir(dfd);
    return FALSE;
}
 
static void parse_lib_symbol_info(const char *symbol, const char *libname)
{
    if ('\0' == _dym_symbol_dir[0])
    {
        parse_func_info(symbol);
        return;
    }
 
    char funcname[256];
    if (FALSE == get_funcname(symbol, funcname))
    {
        format_output_info(symbol, NULL, 0);
        return;
    }
 
    char libpath[256];
    memset(libpath, 0x0, sizeof(libpath));
    if (FALSE == get_lib_symbol_path(libname, libpath))
    {
        format_output_info(funcname, NULL, 0);
        return;
    }
 
    ///parse symbol lib info
}
/// format string end
 
static int malloc_info_map_status(void)
{
    THREAD_AUTOLOCK;
    size_t size = _malloc_info_map.size();
    printf("== unfree pointer count:%ld ==\n", size);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    if (0 != size)
    {
        _frame_pos = 0;
        printf("all alloc memory info:\n");
        std::map<const void *,  BtInfo_t>::iterator _it;
        size_t count = 0;
        size_t memsize = 0;
        for (_it = _malloc_info_map.begin(); _it != _malloc_info_map.end(); ++_it)
        {
            BtInfo_t *pstBtInfo = &(_it->second);
            if (NULL == pstBtInfo)
            {
                printf("<< memory address:\t\t%p, size: unknow >>\n\n", _it->first);
                continue;
            }
            printf("<< \t(%ld) memory address:\t%p, size: %ld, thread_id(0x%lx), p_id(0x%x)\t>>\n", ++count, _it->first, pstBtInfo->mem_size, pstBtInfo->tid, pstBtInfo->pid);
            printf("\t(%ld) backtrace information(%ld):\n", count, pstBtInfo->size);
            memsize += pstBtInfo->mem_size;
            size_t i = 0;
            char libname[256];
            for (i = 0; i < pstBtInfo->size; ++i)
            {
                if (NULL == pstBtInfo->str_bt[i])
                {
                    continue;
                }
 
                memset(libname, 0x0, sizeof(libname));
                if (TRUE == get_libname(pstBtInfo->str_bt[i], libname))
                {
                    ///trans lib info
                    parse_lib_symbol_info(pstBtInfo->str_bt[i], libname);
                }
                else
                {
                    parse_func_info(pstBtInfo->str_bt[i]);
                }
            }
 
            printf("== all unfree memory time(0x%lx) size(0x%lx) ==\n", count, memsize);
        }
    }
    return TRUE;
}
 
static void _private_hook_back()
{
#if __HOOK__
    old_malloc   = __malloc_hook;
    old_free     = __free_hook;
	old_realloc  = __realloc_hook;
	old_memalign = __memalign_hook;
#endif
}
 
static void _private_hook_init()
{
#if __HOOK__
    __malloc_hook   = my_malloc;
    __free_hook     = my_free;
	__realloc_hook  = my_realloc;
	__memalign_hook = my_memalign;
#endif
}
 
static void _private_hook_restore()
{
#if __HOOK__
    __malloc_hook   = old_malloc;
    __free_hook     = old_free;
	__realloc_hook  = old_realloc;
	__memalign_hook = old_memalign;
#endif
}
 
#if __HOOK__
static void *my_malloc(size_t size, const void *caller)
#else
void *__wrap_malloc(size_t size)
#endif
{
    static int count = 0;
    THREAD_AUTOLOCK;
    void *p = NULL;
    _private_hook_restore();
    p = malloc(size);
    //printf("xx malloc time(%d) xx\n", ++count);
    save_malloc_pointer_info(p, size);
    _private_hook_init();
    return p;
}
 
#if __HOOK__
static void my_free(void *ptr, const void *caller)
#else
void __wrap_free(void *ptr)
#endif
{
    static int count = 0;
    THREAD_AUTOLOCK;
    _private_hook_restore();
    remove_free_pointer_info(ptr);
    free(ptr);
    //printf("xx free time(%d) xx\n", ++count);
    _private_hook_init();
}
 
/*
如果是将分配的内存扩大，则有以下情况：
1）如果当前内存段后面有需要的内存空间，则直接扩展这段内存空间，realloc()将返回原指针。
2）如果当前内存段后面的空闲字节不够，那么就使用堆中的第一个能够满足这一要求的内存块，将目前的数据复制到新的位置，并将原来的数据块释放掉，返回新的内存块位置。
3）如果申请失败，将返回NULL，此时，原来的指针仍然有效。
*/
static void *my_memalign(size_t boundary, size_t size, const void *caller)
{
    static int count = 0;
    THREAD_AUTOLOCK;
    void *p = NULL;
    _private_hook_restore();
    p = malloc(size);
    //printf("xx memalign time(%d) xx\n", ++count);
    save_malloc_pointer_info(p, size);
    _private_hook_init();
    return p;
}
 
/*
如果是将分配的内存扩大，则有以下情况：
1）如果当前内存段后面有需要的内存空间，则直接扩展这段内存空间，realloc()将返回原指针。
2）如果当前内存段后面的空闲字节不够，那么就使用堆中的第一个能够满足这一要求的内存块，将目前的数据复制到新的位置，并将原来的数据块释放掉，返回新的内存块位置。
3）如果申请失败，将返回NULL，此时，原来的指针仍然有效。
*/
#if __HOOK__
static void *my_realloc(void *__ptr, size_t size, const void *caller)
#else
void *__wrap_realloc(void *__ptr, size_t size)
#endif
{
    THREAD_AUTOLOCK;
    void *p = NULL;
    _private_hook_restore();
    //printf("realloc size: %ld\n", size);
    void *ptr = __ptr;
    p = realloc(__ptr, size);
    if (NULL != p)
    {
        remove_free_pointer_info(ptr);
        save_malloc_pointer_info(p, size);
    }
    _private_hook_init();
    return p;
}
 
void *__wrap_calloc(size_t numElements, size_t sizeOfElement)
{
    static int count = 0;
    THREAD_AUTOLOCK;
    void *p = NULL;
    _private_hook_restore();
    p = calloc(numElements, sizeOfElement);
    //printf("xx calloc time(%d) xx\n", ++count);
    save_malloc_pointer_info(p, numElements * sizeOfElement);
    _private_hook_init();
    return p;
}
 
static void my_mempool_destroy()
{
    THREAD_AUTOLOCK;
    _private_hook_restore();
}
 
static void my_mempool_init()
{
    THREAD_AUTOLOCK;
    _private_hook_back();
    _private_hook_init();
    //atexit(my_mempool_destroy);
}
 
pthread_t monitor_id = -1;
int thread_exit = FALSE;
 
void *malloc_size_monitor(void *args)
{
    while (FALSE == thread_exit)
    {
        if (_malloc_size_check_point <= _malloc_info_map.size())
        {
            printf("== to becarefully, check point detect, malloc size(%ld) ==\n", _malloc_info_map.size());
            malloc_info_map_status();
        }
 
        usleep(1000 * 1000);
    }
 
    return NULL;
}
 
static void malloc_size_monitor_thread_create(void)
{
    THREAD_AUTOLOCK;
    _private_hook_restore();
    if (0 != pthread_create(&monitor_id, NULL, malloc_size_monitor, NULL))
    {
        monitor_id = -1;
    }
    _private_hook_init();
}
 
static void malloc_size_monitor_thread_destroy(void)
{
    THREAD_AUTOLOCK;
    _private_hook_restore();
    thread_exit = TRUE;
    if (-1 != monitor_id)
    {
        pthread_join(monitor_id, NULL);
    }
    _private_hook_init();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// extern function
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (__HOOK__ == 1)
void (*__MALLOC_HOOK_VOLATILE __malloc_initialize_hook) (void) = my_mempool_init;
#endif
__attribute((constructor)) void CCheckMemoryLeakInit(void)
{
    printf("== Check Memory Leak Init ==\n");
    memset(_dym_symbol_dir, 0x0, sizeof(_dym_symbol_dir));
    malloc_size_monitor_thread_create();
}
 
__attribute((destructor)) void CCheckMemoryLeakDeInit(void)
{
    malloc_size_monitor_thread_destroy();
    my_mempool_destroy();
    malloc_info_map_status();
    printf("== Check Memory Leak Exit ==\n");
}
 
void CCheckMemoryLeakInfo(void)
{
    malloc_info_map_status();
}
 
void CCheckMemoryLeakSetDymSymbolDir(const char *dir)
{
    if (NULL != dir)
    {
        memcpy(_dym_symbol_dir, dir, sizeof(_dym_symbol_dir));
    }
}
 
void CCheckMemoryLeakSetMallocSizeCheckPoint(size_t size)
{
    _malloc_size_check_point = size;
}
 
size_t CCheckMemoryLeakGetMallocSizeInfo(void)
{
    return _malloc_info_map.size();
}

