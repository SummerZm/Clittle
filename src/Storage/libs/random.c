/*************************************************************************
 > File Name: random.c
 > Author: Leafxu
 > Created Time: Wed Oct 24 12:25:24 2018
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>
#include<errno.h>

unsigned int random_int()
{
	int seed=0;
    int rfd = -1;
    unsigned int random_number = 0;
	if ((rfd=open("/dev/urandom", O_RDONLY))!=-1){
try_again:
		if (read(rfd, (void*)&seed, sizeof(seed))==-1){
			if (errno==EINTR) goto try_again; /* interrupted by signal */
		}
		close(rfd);
	}else{
		printf("could not open /dev/urandom (%d)\n", errno);
	}
	seed+=getpid()+time(0);
	srand(seed);
    random_number = rand();
    return random_number;
}

unsigned int random_rang(unsigned int start, unsigned int end)
{
    unsigned int number = 0;
    unsigned int mod_val = end -start;
    if(mod_val<=0) {
        printf("Error: end must large than start\n");
        return -1;
    }
    number = random_int();
    number = number%mod_val;
    number += start;
    return number;
}
