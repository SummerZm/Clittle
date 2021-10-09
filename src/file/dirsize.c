/*************************************************************************
 > File Name: dirsize.c
 > Author: Leafxu
 > Created Time: Fri 09 Aug 2019 03:22:30 PM CST
 ************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

#define MAX_PATH_LEN	8024	

long getFileSize(const char* fileName) {
	struct stat info;
	if(fileName == NULL) return -1;
	if(stat(fileName, &info)<0) return -1;
	return info.st_size;
}

int isDirFile(const char* fileName) {
	struct stat info;
	if(fileName == NULL) return -1;
	if(stat(fileName, &info)<0) return -1;
	return S_ISDIR(info.st_mode);
}


long getDirSize(const char* dirPath) {
	int total=0;
	DIR* dirFd=NULL;
	struct dirent *entry= NULL;
	char filePath[MAX_PATH_LEN];
	if(dirPath == NULL) return -1;
	if( (dirFd=opendir(dirPath)) == NULL) return -1;

	while((entry=readdir(dirFd)) != NULL) {
		errno=0;
		if(strcmp(entry->d_name, ".")==0) {
			total += 4095;
			continue;
		}
		if(strcmp(entry->d_name, "..")==0) {
			continue;
		}

		sprintf(filePath,"%s/%s", dirPath, entry->d_name);
		if(isDirFile(filePath)==1) {
			//printf("filepath : %s; recursion...\n\n", entry->d_name);
			total += getDirSize(filePath);
		}
		else {
			total += getFileSize(filePath);
			//printf("filename : %s; size: %d\n", filePath, getFileSize(filePath));
		}
	}
	if(errno>0) perror("readdir error");
	else {
		printf("Dir %s total size: %d\n", dirPath, total);
		printf("Dir %s total size: %dK\n", dirPath, total>>10);
	}
	return total;
}


int main(int argc, char** argv) {
	//printf("file %d \n", getFileSize("test"));
	//printf("filedir %d \n", isDirFile("test1/test"));
	printf("filedir %d \n", getDirSize("/work/Leafxu/demo/"));
	return 0;
}


