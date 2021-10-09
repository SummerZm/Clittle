/*************************************************************************
 > File Name: intarray.h
 > Author: Leafxu
 > Created Time: Wed Oct 24 10:33:33 2018
 ************************************************************************/
#ifndef _INT_ARRAY_H_
#define _INT_ARRAY_H_

struct intArray
{
    int* arr; 
    int size;
    int capacity;
};

struct intArray* int_array(int size);
  
int int_array_grow(struct intArray* list);

// index [0, capacity]
int int_array_add(struct intArray* list, int index, int val);

int int_array_find(struct intArray* list, int val);

int int_array_reduce(struct intArray* list);

int int_array_remove(struct intArray* list, int val);

int int_array_print(struct intArray* list);

struct intArray* test_int_array();

struct intArray* test_int_array_add();

struct intArray* test_int_array_print();

struct intArray* test_int_array_remove();

int test_int_array_find();

//int main(int agrc, char** agrv);
#endif
