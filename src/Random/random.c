/*************************************************************************
 > File Name: random.c
 > Author: Leafxu
 > Created Time: Mon 22 Oct 2018 09:30:49 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>


// random1
int random_str()
{
    FILE *fpr = NULL;
    char buff[1024] = {0};
    //char* cmd = "echo $RANDOM | md5sum | cut -c 5-11";
    //char* cmd = "openssl rand -base64 30";
    //char* cmd = "date +%s%N";
    //char* cmd = "head /dev/urandom | cksum";
    char* cmd = "cat /proc/sys/kernel/random/uuid";
    fpr = popen(cmd,"r");
    if(fpr) {
        fread(buff, 1, sizeof(buff), fpr);
        printf("random str: %s", buff);
        pclose(fpr);
    }
    return 0;
}

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
	//printf("seeding PRNG with %u\n", seed);
	srand(seed);
    random_number = rand();
	//printf("test random number %u\n", rand());
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



int main(int argc, char** agrv)
{
    //random_str();
    //unsigned int random_number = random_int();
    unsigned int random_number = random_rang(100, 119);
    printf("random_number: %d\n", random_number);
    return 0;
}

