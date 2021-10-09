/*************************************************************************
 > File Name: fcntl.c
 > Author: Leafxu
 > Created Time: Sat 05 May 2018 05:45:54 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

union flag{
    int i;
    char b[4];
};

void set_fl(int fd, int flag) 
{
    union flag val; 
    if ((val.i = fcntl(fd, F_GETFL)) < 0) {
        printf("get fd stat fail \n");
    }
    else {
        printf("get fd stat %d \n", (val.i));
    }

    val.i |= flag; 
    //printf("get fd stat %d \n", (val.i));
    if (fcntl(fd, F_SETFL, val.i) < 0) {
        printf("set fd stat fail\n");
    } 
    else {
        printf("set fd stat success\n");
    }
}

int main(int argc, char** argv) 
{
    int fd;
    fd = open("./fcntl.c", O_RDWR|O_APPEND);
    if (fd == -1)  {
        printf("open file fail \n");
        return 0;
    }

    set_fl(fd, O_SYNC);
    if (close(fd) == -1) {
        printf("close fail\n");
    }
    return 0;

}





































































