/*************************************************************************
 > File Name: storage.h
 > Author: Leafxu
 > Created Time: Fri 12 Oct 2018 02:25:07 PM CST
 ************************************************************************/
#ifndef _STORAGE_H_
#define _STORAGE_H_
// 存储结构--访问的性能--malloc
/*struct node {
    void* data;         // data domain
    struct node* next;  // struct domain [tree - linklist - other] 
};

// 线性表的两种基本存储结构  单链表[修改] -- 数组[随机访问]
union storage {
    struct node head;   // 链表
    void* arr;          // 堆数组
};*/

// 存储操作 -- 构造，增，删，改，查, 析构
/*struct storage_op {
};*/

//typedef int (*add)(void* list, void* data);
//typedef int (*remove)(void* list, void* data);
//typedef void* (*find)(void* list, void* data);
//typedef int (*modify)(void* list, void* data);

//typedef void* (*storage_op)(void*, void*);
//typedef int (*storage_ops)(void*, void*);

// 类似于面向对象中的接口
/*struct operations {
    // 初始化[分配]/销毁数据域  -- 传入数据域指针
    // init 决定了数据的存储结构
    storage_op init;      
    storage_op destory;

    // 增，删，改，查
    storage_op add;
    storage_op remove;
    storage_op find;
    storage_op modify;

    // 比较，拷贝，排序
    storage_op compare;
    storage_op clone;
    storage_op sort;
};*/

struct storage {
    // union storage data;   
    // Basic storage struct [heap array or linklist].           
    // 线性表的两种基本存储结构  单链表[修改] -- 数组[随机访问]
    void *storage;          

    // Basic storage oprerations.
    // 不同数据结构的操作集
    // void* op;   

    // If storage type is link, capacity equal to size.
    int capacity;           

    // The number of data we have in storage.
    int size;               

    // 辅助访问结构体 -- 头指针, 索引表等
    void *assist;

    /* 简化代码结构
    #define Add op->add
    linklist.Add(NULL, NULL);
    #undef Add
    */
};

//extern int storage(struct storage* container, void* data, void* op, int capacity, int size);
//extern int storage(struct storage* container, void* data, int capacity, int size);
extern int storage(struct storage* container, void* data, void* assist, int capacity, int size);
#endif


