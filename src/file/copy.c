/*************************************************************************
 > File Name: copy.c
 > Author: Leafxu
 > Created Time: Sat 05 May 2018 05:00:11 AM PDT
 ************************************************************************/
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    int inputFd, outputFd, openFlag;
    mode_t filePerms;
    size_t numRead;
    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        printf("%s old-file new-file\n", argv[0]);
        return 0;
    }

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        printf("opening file %s error\n", argv[1]);
        return 0;
    }

    openFlag = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    outputFd = open(argv[2], openFlag, filePerms);
    if (outputFd == -1) {
        printf("opening file %s error\n", argv[2]);
        return 0;
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            printf("Can't write whole buffer\n");
            break;
        }
    }

    if (numRead == -1) {
        printf("read error\n");
    } 
    
    // 检查是否正常关闭，确保文件内容被写入磁盘中
    if(close(inputFd) == -1) {
        printf("inputFd close fail\n");
    }

    if(close(outputFd) == -1) {
        printf("outputFd close fail\n");
    }

    return 0;

}



