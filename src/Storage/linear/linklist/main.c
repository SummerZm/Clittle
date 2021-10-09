/*************************************************************************
 > File Name: main.c
 > Author: Leafxu
 > Created Time: Fri 12 Oct 2018 02:53:08 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "storage.h"
#include "linklist.h"

// 没有维护storage的size, capacity
int main(int argc, char** agrv) {
    struct storage linklist;
    storage(&linklist, NULL, &head, 0, 0);
    linklist_op.init(&linklist, NULL);
    
    //struct linklist* Node1;
    //Node1 = (struct linklist*)node_create(1);
    //linklist_op.add(linklist.storage, Node1);
    //Node1 = (struct linklist*)node_create(2);
    //linklist_op.add(linklist.storage, Node1);
    struct linklist_assist* assist = (struct linklist_assist*)linklist.assist;

    int arr[] = {0,1,2,3};
    linklist_op.add(linklist.storage, (int*)arr);
    linklist_op.add(linklist.storage, (int*)arr+1);
    linklist_op.add(assist->head, (int*)arr+2);
    linklist_op.add(assist->head, (int*)arr+3);
    //linklist_op.remove(assist->head, (int*)arr+3);

    linklist_op.print(&linklist, NULL); 
    linklist_op.destory(&linklist, NULL);

    //Node1 = NULL;
    return 0;
}
