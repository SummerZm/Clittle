/*************************************************************************
 > File Name: linklist.c
 > Author: Leafxu
 > Created Time: Thu 11 Oct 2018 07:53:57 PM CST
 ************************************************************************/

#include <stdio.h>
#include "storage.h"


//extern int storage(struct storage* container, void* data, void* op, int capacity, int size) {
extern int storage(struct storage* container, void* data, void* assist, int capacity, int size) {
    container->storage = data;
    //container->op = op;
    container->capacity = capacity;
    container->size = size;
    container->assist = assist;
    //container->op->init(container->storage, NULL);
    return 0;
}


// 排序的函数指针 -- 底层排序可修改
//typedef int (*node_sort)(struct list*);
//typedef int (*list_init)();
//typedef int (*list_storge_update)();
//typedef int (*list_storge_op_update)();

//typedef int (*compare)(struct list*, struct list*);
// 使用typeof是泛型的一种实现方式
// 实现操作函数集 -- 传入一个函数指针
// 将数据存储与数据操作分离 --- 数据结构与操作技巧
// 对结构的基础操作: add, remove, search, modify
// 基础操作的具体逻辑:


//int add();
//int remove();
//int search();
//int modify();

/*int sort() {
    return 0;
}*/

/*int main(int argc, char** argv) {
    return 0;
}*/
