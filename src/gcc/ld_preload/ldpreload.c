/*************************************************************************
 > File Name: ldpreload.c
 > Author: Leafxu
 > Created Time: Tue 23 Jul 2019 10:00:30 AM CST
 ************************************************************************/

#include<stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
uid_t geteuid( void ) { return 0; }
uid_t getuid( void ) { return 0; }
uid_t getgid( void ) { return 0; }
