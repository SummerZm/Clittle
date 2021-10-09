/*************************************************************************
 > File Name: tree.c
 > Author: Leafxu
 > Created Time: Wed 17 Oct 2018 07:49:23 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include "tree.h"

struct node* make_node(int val)
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    if(node) {
        node->val = val;
        node->count = 1;
        node->left = NULL;
        node->right = NULL;
    }
    else{
        printf("make_node fail\n");
    }
    return node;
}

int BSTree_add(struct BSTree* tree, struct node** root, struct node* node) 
{   
    if(tree==NULL || node==NULL || root==NULL) {
        printf("BSTree_add() ---> BSTree or node is null\n");
        return -1;
    }

    //printf("===================================\n");
    if(*root == NULL) {
        *root = node;
        tree->size++;
        tree->depth++;
        printf("add success %d, tree size: %d\n", node->val,tree->size);
        return 0;
    }
    struct node* _root = *root;
    //printf("===================================\n");
    //printf("\nroot:%d\n", _root->val);

    if(_root->val > node->val) {
        //printf("===================================\n");
        //printf("\nleft:%d\n", _root->val);
        BSTree_add(tree, &(_root->left), node);
    }
    else if(_root->val < node->val){
        //printf("\nright:%d\n", _root->val);
        BSTree_add(tree, &(_root->right), node);
    }
    else{
       _root->count++;
    }
}

void print_BSTree(struct node* root, int depth) 
{
    int spaces = depth;

    if(root==NULL) {
        while(spaces>0) {
            printf("--");
            spaces--;
        }
        //printf("#\n");
        printf(">\n");
        return;
    }
    
    depth++;   // on top forever
    print_BSTree(root->left, depth);
    while(spaces>0) {
        printf("  ");
        spaces--;
    }
    printf("%d(%d)\n", root->val, root->count);
    print_BSTree(root->right, depth);
}


void test_make_node()
{
    printf(">>>>>>>>>>test_make_node()\n");
    struct node* node = make_node(1);
    if(node) {
        printf("make_node: %d\n", node->val);
    }
    free(node);
    return;
}

void test_BSTree_add()
{
    printf(">>>>>>>>>>test_BSTree_add()\n");
    struct BSTree tree;
    tree.size = 0;
    tree.node = NULL;
    tree.depth = 0;
    //struct node* node6 = make_node(1);
    struct node* node1 = make_node(4);
    struct node* node2 = make_node(2);
    struct node* node3 = make_node(3);
    struct node* node5 = make_node(7);
    struct node* node6 = make_node(8);
    printf(">>>>>>>>>>test_BSTree_add()\n");
    BSTree_add(&tree, &tree.node, node1); 
    BSTree_add(&tree, &tree.node, node2); 
    BSTree_add(&tree, &tree.node, node3); 
    BSTree_add(&tree, &tree.node, node5); 
    BSTree_add(&tree, &tree.node, node6); 
    print_BSTree(tree.node, 1);
}


int main(int argc, int argv)
{
    //test_make_node();
    test_BSTree_add();
    return 0;
}

