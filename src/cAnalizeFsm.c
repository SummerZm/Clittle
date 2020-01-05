/*************************************************************************
 > File Name: fsm.c
 > Author: Leafxu
 > Created Time: Thu 19 Apr 2018 10:52:46 PM CST
 > Desc: 有限状态机 - "非常实用的编程手法" - 词法分析 - from C expert book
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXTOKENS   100
#define MAXTOKENLEN 64

/*
 * IDENTIFIER 标识符
 * QUALIFIER  形容词
 * TYPE 类型
 * */
enum type_tag {IDENTIFIER, QUALIFIER, TYPE};

struct token {
    char type;
    char string[MAXTOKENLEN];
};

int top = -1;

/* 保存第一个标志符前所有标记 */
struct token stack[MAXTOKENS];

/* 保存刚读入的标记 */
struct token this;

#define pop stack[top--]
#define push(s) stack[++top]=s

/* 判断标识符的类型 */
enum type_tag classify_string(void)
{
    char *s = this.string;
    if (!strcmp(s, "const")) {
        strcpy(s,"readonly");
        return QUALIFIER;
    }

    if (!strcmp(s, "volatile")) return QUALIFIER;
    if (!strcmp(s, "void")) return TYPE;
    if (!strcmp(s, "char")) return TYPE;
    if (!strcmp(s, "signed")) return TYPE;
    if (!strcmp(s, "unsigned")) return TYPE;
    if (!strcmp(s, "short")) return TYPE;
    if (!strcmp(s, "int")) return TYPE;
    if (!strcmp(s, "long")) return TYPE;
    if (!strcmp(s, "float")) return TYPE;
    if (!strcmp(s, "double")) return TYPE;
    if (!strcmp(s, "struct")) return TYPE;
    if (!strcmp(s, "union")) return TYPE;
    if (!strcmp(s, "enum")) return TYPE;

    return IDENTIFIER;
}

/* 从标准输入中逐字符的读取数据 */
void gettoken(void)
{
    char *p = this.string;
    /* 略过所有空白字符 */
    while ((*p = getchar()) == ' ');
    if (isalnum(*p)) {
        /* 读入所有a-z, 1-9字符 */
        while(isalnum(*++p = getchar()));
        ungetc(*p, stdin);
        *p = '\0';
        this.type = classify_string();
        return;
    }
    this.string[1] = '\0';
    this.type = *p;
    return;
}

/* 状态函数 */
void initialize(), get_array(), get_parms(), get_lparen(),
get_ptr_part(), get_type();

void (*nextstate)(void) = initialize;

int main(int argc, char **agrv)
{
    /* 在不同的状态中切换, 直到指针为null */      
    while (nextstate != NULL) 
        (*nextstate)();
    return 0;
}

/* 获标识符 */
void initialize()
{
    gettoken();
    while (this.type != IDENTIFIER) {
        push(this); /* 入栈 */
        gettoken();
    }
    printf("%s is ", this.string);
    gettoken();
    nextstate = get_array;
}


/* 判断是函数还是数组 */
void get_array()
{
    nextstate = get_parms;
    while (this.type == '[') {
        printf("array ");
        gettoken();  /* 读取一个数字，或']' */
        if (isdigit(this.string[0])) {
            printf("0..%d ", atoi(this.string) - 1);
            gettoken(); /* 读取 ']' */
        }
        gettoken(); /* 读取']'之后的数据 */
        printf("of ");
        nextstate = get_lparen;
    }
}

/* 处理函数参数 */
void get_parms()
{
    nextstate = get_lparen;
    if (this.type == '(') {
        while (this.type != ')') {
            gettoken();
        }
        gettoken();
        printf("function returning ");
    }
}

/* 处理栈 - 直到栈空 */
void get_lparen()
{
    nextstate = get_ptr_part;
    if(top >= 0){
        if (stack[top].type == '(') {
            pop;
            gettoken();
            nextstate = get_array;
        }
    }
}


void get_ptr_part()
{
    nextstate = get_type;
    /* 指针 */
    if (stack[top].type == '*') {
        printf("point to ");
        pop;
        nextstate = get_lparen;
    }
    /* 形容词 */
    else if (stack[top].type == QUALIFIER) {
        printf("%s ", pop.string);
        nextstate = get_lparen;
    }
}

void get_type() 
{
    nextstate = NULL;
    /* 处理读入标识符之前，存放在stack里的标记 */
    while (top >= 0) {
        printf("%s ", pop.string);
    }
    printf("\n");
}









