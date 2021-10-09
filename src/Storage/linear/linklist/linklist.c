/*************************************************************************
 > File Name: linklist.c
 > Author: Leafxu
 > Created Time: Fri 12 Oct 2018 05:14:25 PM CST
 ************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "linklist.h"

int linklist_init(struct storage* list, struct linklist* node)
{
    printf("linklist_init\n");
    if(NULL == list) {
        printf("linklist_init fail -- storage is null\n");
        return -1;
    }
    
    if(NULL == node && !(node=(struct linklist*)malloc(sizeof(struct linklist))) ) {
        printf("linklist_init fail -- malloc linklist fail\n");
        return -1;
    }

    node->next = NULL;
    list->storage = node;
    ((struct linklist_assist*)list->assist)->head = node;
    return 0;
}

struct linklist* node_create(int val) 
{
    struct linklist* node = (struct linklist*)malloc(sizeof(struct linklist));
    int* data = (int*)malloc(sizeof(int));
    *data = val;
    if(node && data) {
        node->data = data;
        node->size = sizeof(int);
        node->next = NULL;
        //printf("create success\n");
        return node;
    }
    else {
        if(node) free(node);
        if(data) free(data);
        //printf("create fail\n");
        return NULL;
    }
}

int node_destory(struct linklist* node) 
{
    if(node && node->data) {
        printf("node_destory success\n");
        free(node->data);
        node->size = 0;
    }
    return 0;
}

int linklist_destory(struct storage* list, struct linklist* node) 
{
    //printf("linklist_destory\n");
    struct linklist* temp = NULL;

    if(NULL == list) {
        printf("linklist_destory fail -- storage is null\n");
        return -1;
    }

    node = (struct linklist*)(list->storage);
    while(node) {
        temp = node->next;
        node_destory(node);
        free(node);
        node = temp;
    }

    // 用指针清空一个全局变量的值
    ((struct linklist_assist*)list->assist)->head = NULL;
    list->assist = NULL;
    printf("linklist_destory success\n");
    return 0;
}


int linklist_compare(struct linklist* object1, struct linklist* object2) {
    printf("linklist_compare\n");
    int* val1 = (int*)object1->data;
    int* val2 = (int*)object2->data;
    if(*val1-*val2>0)
        return 0;
    return -1;
}

int linklist_sort(struct storage* list, struct linklist* node) {
    printf("linklist_sort\n");
    return 0;
}

int linklist_clone(struct storage* list, struct linklist* node) {
    printf("linklist_clone\n");
    return 0;
}

int linklist_add(struct linklist* pre_node, int* val)
{
    //printf("linklist_add\n");
    struct linklist* temp = NULL;
    struct linklist* node = NULL;

    if(NULL == pre_node) {
        printf("linklist_add fail -- pre_node is null\n");
        return -1;
    }

    node = node_create(*val);
    if(node == NULL) {
        printf("linklist_add fail -- node is null\n");
        return -1;
    }

    temp = pre_node->next;
    pre_node->next = node;
    node->next = temp;
    printf("linklist_add success\n");
    return 0;
}
/*
int linklist_headadd(struct storage* list, struct linklist* node)
{
    //printf("linklist_headadd\n");

    if(NULL==list && NULL==node) {
        printf("linklist_headadd fail -- node is null\n");
        return -1;
    }
    struct linklist_assist* assist = (struct linklist_assist*)list->assist;
    linklist_add(assist, node);
    list->assist->head->next = node;
}*/



int linklist_remove(struct linklist* pre_node, struct linklist* node)
{
    printf("linklist_remove\n");
    struct linklist* temp = NULL;

    if(NULL==pre_node && NULL==node) {
        printf("linklist_add fail -- node is null\n");
        return -1;
    }

    temp = pre_node->next;
    while(temp) {
        if(0 == linklist_compare(temp, node)) {
            pre_node->next = temp->next;
            free(temp);
            break;
        } 
        pre_node = temp;
        temp = pre_node->next;
    }
    return 0;
}

int linklist_find(struct storage* list, struct linklist* node) {
    printf("linklist_find\n");
    return 0;
}

int linklist_modify(struct storage* list, struct linklist* node) {
    printf("linklist_modify\n");
    return 0;
}

int linklist_print(struct storage* list, struct linklist* node) 
{
    //printf("linklist_print\n");
    if(list == NULL) return -1;

    struct linklist_assist* assist = (struct linklist_assist*)list->assist;
    if(assist &&  assist->head) {
        int* val = NULL;
        struct linklist* head = assist->head;
        while(head->next){
            val = (int*)(head->next->data);
            printf("-->%d", *val);
            head = head->next;
        }
        printf("\n");
        return 0;
    }
    /*  Ok also.
    if(list && list->storage) {
        int* val = NULL;
        struct linklist* head = (struct linklist*)list->storage;
        while(head->next) {
            val = (int*)(head->next->data);
            printf("%d\n", *val);
            head = head->next;
        }
        return 0;
    }*/
    return -1;
}


struct operations linklist_op = { 
    .init = (storage_op)linklist_init,
    .destory = (storage_op)linklist_destory,
    .add = (storage_op)linklist_add,
    .remove = (storage_op)linklist_remove,
    .find = (storage_op)linklist_find,
    .modify = (storage_op)linklist_modify,
    .compare = (storage_op)linklist_compare,
    .clone = (storage_op)linklist_clone,
    .sort = (storage_op)linklist_sort,
    .print = (storage_op)linklist_print,
};

struct linklist_assist head = {
    .head = NULL,    
};







