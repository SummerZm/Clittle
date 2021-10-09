/*************************************************************************
 > File Name: tree.h
 > Author: Leafxu
 > Created Time: Wed 17 Oct 2018 07:49:59 PM CST
 ************************************************************************/
#ifndef _TREE_H_
#define _TREE_H_

struct node
{
    int val;
    int count;
    struct node* left;
    struct node* right;
};

// binary search tree
struct BSTree
{
    struct node* node;
    int size;
    int depth;
};


#endif

