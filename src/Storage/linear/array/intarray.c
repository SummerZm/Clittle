/*************************************************************************
 > File Name: intarray.c
 > Author: Leafxu
 > Created Time: Mon 22 Oct 2018 08:04:47 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include"intarray.h"

struct intArray* int_array(int size)
{
    struct intArray* container = (struct intArray*)malloc(sizeof(struct intArray));
    if(container != NULL) {
        if(size >0) {
            container->arr = (int*)malloc(size*sizeof(int));
        }else { 
            container->arr = NULL;
        }

        if(container->arr == NULL) {
            container->size = 0;
        }else {
            container->size = size;
        }
        container->capacity = 0;
    }
    else {
        printf("Fail: Malloc struct intArray failed\n");
    }
	return container;
}
  
int int_array_grow(struct intArray* list)
{
    if(list == NULL){
        printf("Error: int_array_grow() intArray=null\n");
        return -1;
    }

    int i;
    int size = list->size;
    int* arr = list->arr;
    int capacity = list->capacity;
    int* newSpace = (int*)malloc(2*size*sizeof(int));

    if(newSpace !=NULL) {
        for(i=0; i<capacity; i++) 
			newSpace[i] = arr[i];
        free((int (*)[list->size])(list->arr));
        list->arr = newSpace;
        list->size = 2*size;
        printf("Success: int_array_grow()\n");
    }
    else {
        printf("Fail: int_array_grow() malloc failed\n");
        return -1;
    }
    return 0;
}

// index [0, capacity]
int int_array_add(struct intArray* list, int index, int val)
{
    if(list==NULL || index<0 || index>list->capacity) {
        printf("Error: int_array_add(): list or index illegal\n");
        return -1;
    }

    int i = 0;
    int capacity = list->capacity;
    int size = list->size;
	//int* arr = list->arr; error --- because arr will change when int_array_grow().
   
    if(capacity+1 <= size || -1!=int_array_grow(list)) {
		int* arr = list->arr;
		if(capacity > 0) i = capacity;

		while(i>index) {
			arr[i] = arr[i-1];
			i--;
		}

		arr[index] = val;
        list->capacity++;
        printf("Success: int_array_add() index[%d], add[%d] capacity[%d]\n", index, arr[index], list->capacity);
    }
    else{
        printf("Error: int_array_add(): grow failed\n");
        return -1;
    }
    return 0;
}

int int_array_find(struct intArray* list, int val)
{	
	int index = -1;
	if(list==NULL) {
		printf("Error: int_array_find() list is null\n");
		return index;
	}

	int i;
	for(i=0; i<list->capacity; i++) {
		if(list->arr[i] == val)
			index = i;
	}
	return index;
}

int int_array_reduce(struct intArray* list)
{
	int i=0;
	if(list==NULL || 4*list->capacity>list->size) {
		return -1;
	}

	int* newSpace = (int*)malloc(2*list->capacity);
	if(newSpace != NULL){
		for(i=0; i<list->capacity; i++) {
			newSpace[i] = list->arr[i];
		}
		free(list->arr);
		list->arr = newSpace;
		list->size = 2*list->capacity;
		printf("Success: int_array_reduce() now size %d\n", list->size);
	}
	else{
		printf("Error: int_array_reduce() malloc failed\n");
		return -1;
	}
	return 0;
}

int int_array_remove(struct intArray* list, int val)
{
	if(list==NULL) {
		printf("Error: int_array_remove(): list is null\n");
		return -1;
	}
	
	int index = int_array_find(list, val);
	if(index>=0) {
		while(index<list->capacity-1) {
			list->arr[index] = list->arr[index+1];
			index++;
		}
		list->capacity--;
		printf("Success: int_array_remove() val[%d]\n", val);
		int_array_reduce(list);
		return 0;
	}
	else{
		return -1;
	}
}

int int_array_print(struct intArray* list)
{
	if(list == NULL || list->arr == NULL) {
		return -1;
	}

	int i = 0;
	while(i<list->capacity) {
		printf("arr[%d]=[%d]\n", i, list->arr[i]);
		i++;
	}
	return 0;
}

int int_array_destory(struct intArray* list)
{
	int ret = -1;
	if(list != NULL) {
		if(list->arr) {
			free(list->arr); 
			list->arr = NULL;
		}
		list->size = 0;
		list->capacity = 0;
		ret = 0;
	}
	else {
		printf("intArray is null\n");
	}
	return ret;
}


struct intArray* test_int_array() 
{
    struct intArray* testarr = int_array(4);
    return testarr;
}

struct intArray* test_int_array_add()
{
    //struct intArray* test = int_array(2);
	struct intArray* test = test_int_array();
    /*int_array_add(test, 0, 1);
    int_array_add(test, 0, 2);*/
    int_array_add(test, test->capacity, 1);
    int_array_add(test, test->capacity, 2);
    int_array_add(test, test->capacity, 3);
    int_array_add(test, test->capacity, 4);
    int_array_add(test, test->capacity, 5);
    int_array_add(test, test->capacity, 6);
    int_array_add(test, test->capacity, 7);
    return test;
}


struct intArray* test_int_array_print()
{
	struct intArray* test = test_int_array_add();
	int_array_print(test);
	return test;
}

struct intArray* test_int_array_remove()
{
	struct intArray* test = test_int_array_add();
	int_array_remove(test, 1);
	int_array_remove(test, 2);
	int_array_remove(test, 3);
	int_array_remove(test, 4);
	int_array_remove(test, 5);
	int_array_remove(test, 6);
	int_array_print(test);
	return test;
}

int test_int_array_find()
{
	struct intArray* test = test_int_array_add();
	int ret = int_array_find(test, 8);
	printf("index is %d\n", ret);
	return ret;
}

//int main(int agrc, char** agrv)
int test()
{
    //test_int_array();
    //test_int_array_add();
	//test_int_array_print();
	test_int_array_remove();
	//test_int_array_find();
    return 0;
}





