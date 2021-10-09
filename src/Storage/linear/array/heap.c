/*************************************************************************
 > File Name: heap.c
 > Author: Leafxu
 > Created Time: Wed Oct 24 10:33:19 2018
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<intarray.h>
#include<random.h>
#include<time.h>
//#include"heap.h"

// 泛型堆 (以后实现)
struct intArray* create_heap(int size)
{
	return int_array(size);
}

int heap_capacity(struct intArray* heap)
{
	return heap?heap->capacity:0;
}

int heap_size(struct intArray* heap)
{
	return heap?heap->size:0;
}

int left_node_index(struct intArray* heap, int node_index)
{
	int left_index = 0;
	if(heap==NULL || node_index<0) {
		printf("left_node_index(): heap is null\n");
		return -1;
	}
	left_index = 2*node_index + 1;
	if(left_index < heap->size) {
		return left_index;
	}
	else {
		printf("left_node_index(): index is bigger than capacity\n");
		return -1;
	}
}

int right_node_index(struct intArray* heap, int node_index)
{
	int right_index = 0;
	if(heap==NULL || node_index<0) {
		printf("right_node_index(): heap is null\n");
		return -1;
	}
	right_index = 2*node_index + 2;
	if(right_index < heap->size) {
		return right_index;
	}
	else {
		printf("right_node_index(): index is bigger than capacity\n");
		return -1;
	}
}

int parent_node_index(struct intArray* heap, int node_index)
{
	int parent_index = 0;
	if(heap==NULL || node_index<=0) {
		printf("parent_node_index(): heap is null\n");
		return -1;
	}

	parent_index = (node_index-1)/2;
	return parent_index;
}

int heap_shift_up(struct intArray* heap, int node_index)
{
	int ret = -1;
	int parent_index = 0;
	if(heap == NULL) {
		printf("heap_shift_up(): heap is null\n");
		return ret;
	}
	
	parent_index = parent_node_index(heap, node_index);
	if(parent_index>=0 && heap->arr[node_index] < heap->arr[parent_index]) {
		heap->arr[node_index] *= heap->arr[parent_index];
		heap->arr[parent_index] = heap->arr[node_index] / heap->arr[parent_index];
		heap->arr[node_index] = heap->arr[node_index] / heap->arr[parent_index];
		ret = parent_index;
	}
	return ret;
}

int heap_shift_down(struct intArray* heap, int node_index)
{
	int ret = -1;
	int child_index = 0;
	if(heap == NULL) {
		printf("heap_shift_down():heap is null\n");
		return ret;
	}

	child_index = left_node_index(heap, node_index);
	if(child_index < heap->size && heap->arr[node_index] > heap->arr[child_index]){
		heap->arr[node_index] *= heap->arr[child_index];
		heap->arr[child_index] = heap->arr[node_index] / heap->arr[child_index];
		heap->arr[node_index] = heap->arr[node_index] / heap->arr[child_index];
		ret = child_index;
	}

	child_index = right_node_index(heap, node_index);
	node_index = ret;
	if(child_index < heap->size && heap->arr[node_index] > heap->arr[child_index]){
		heap->arr[node_index] *= heap->arr[child_index];
		heap->arr[child_index] = heap->arr[node_index] / heap->arr[child_index];
		heap->arr[node_index] = heap->arr[node_index] / heap->arr[child_index];
		ret = child_index;
	}
	return ret;
}


// index = 0 fisrt
int heap_add(struct intArray* heap, int val)
{

	// add in last
	// shift up
	/*if(heap==NULL && index<0) {
		printf("heap is null or index illegal\n");
		return -1;
	}

	int size = heap_size(heap);

	if(size == index) {
		int_array_add(heap, index, val);
		return 0;
	}

	// one note 
	if(index+1 = size) {
		if(heap->arr[index] > val) {
			int_array_add(heap, size, val);
			//move node
			heap->arr[index+1] = heap->arr[index];
			heap->arr[index] = val;
		}
	}	// two note
	else if(index+2 = size) {
		// one
		if(heap->arr[index] > val) {
		}
		else if(heap->arr[index+1] > val) { // two
		}
		else{
			//heap_add();
		}
	}
	else { // tree note
	}*/
}

int heap_remove();

int heap_destory();

int heap_printf();

int main(int argc, char** argv)
{
	printf("random: %d\n", random_rang(24, 68));
	unsigned long start = time_nsec();
	test_int_array_remove();
	unsigned long end = time_nsec();
	printf("Run time: %lu ns\n", end-start);
	return 0;
}

