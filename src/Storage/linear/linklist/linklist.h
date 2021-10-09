/*************************************************************************
 > File Name: linklist.h
 > Author: Leafxu
 > Created Time: Fri 12 Oct 2018 05:46:47 PM CST
 ************************************************************************/
#ifndef _LINKLIST_H_
#define _LINKLIST_H_
#include "storage.h"

struct linklist {
    void* data;
    int size;
    struct linklist* next;
};

struct linklist_assist {
    struct linklist* head;
};


typedef void* (*storage_op)(void*, void*);
// 类似于面向对象中的接口
struct operations {
    // 初始化[分配]/销毁数据域  -- 传入数据域指针
    // init 决定了数据的存储结构
    storage_op init;      
    storage_op creat;
    storage_op destory;
    storage_op print;

    // 增，删，改，查
    storage_op add;
    storage_op remove;
    storage_op find;
    storage_op modify;

    // 比较，拷贝，排序
    storage_op compare;
    storage_op clone;
    storage_op sort;
};
extern struct operations linklist_op;
extern struct linklist_assist head;
#endif
