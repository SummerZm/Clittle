/*************************************************************************
 > File Name: sys.c
 > Author: Leafxu
 > Created Time: 2020年07月08日 星期三 13时37分28秒
 ************************************************************************/

#include <sys/sysinfo.h>
// see: man 2 sysinfo
// long uptime;             /* Seconds since boot */
// unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
// unsigned long totalram;  /* Total usable main memory size */
// unsigned long freeram;   /* Available memory size */
// unsigned long sharedram; /* Amount of shared memory */
// unsigned long bufferram; /* Memory used by buffers */
// unsigned long totalswap; /* Total swap space size */
// unsigned long freeswap;  /* Swap space still available */
// unsigned short procs;    /* Number of current processes */
// char _f[22];             /* Pads structure to 64 bytes */

long sysUpTime() {
    struct sysinfo info;
    if (sysinfo(&info)) {
        return -1;
    }
    return info.uptime;
}

long sysCurrTime() {
    struct timeval tv;
    get_time_of_day(&tv, NULL);
    return tv.tv_sec;
}

int executeCommand(const char *cmd, char* buff, int len) {
	FILE *pfd=NULL;
	int total=len, ret=-1;

	if(cmd==NULL || buff==NULL || len<=0) {
		return ret;
	}

    pfd=popen(cmd, "r");
    if (pfd == NULL) {
        printf("cmd %s failed %s\n", cmd, strerror(errno));
        return -1;
    }
	memset(buff, 0, len);
    for (;;) {
        errno = 0;
	    ret = fread(buff, sizeof(char), len, pfd);
        if (ret>0) {
            break;
        }
        else {
            if (errno!=0) {
                printf("cmd %s failed %s\n", cmd, strerror(errno));
                ret = -1;
                break;
            }
            continue;
        }
    }

    if (pclose(pfd)==-1) {
        printf("cmd %s failed %s\n", cmd, strerror(errno));
    }
    return ret;
}


/*
 * @Note: Get file system capacity.
 * @Return: 0/1  
 */
#include <sys/vfs.h>
int  getFSCapacity(const char* path, unsigned long long *total, unsigned long long *free) {
    int ret=-1;
    struct statfs diskInfo;
    if (path==NULL || total==NULL || free==NULL) {
        printf("Argument error\n");
        return ret;
    }
    ret = statfs(path,&diskInfo);
    *total = diskInfo.f_bsize * diskInfo.f_blocks;
    *free = diskInfo.f_bsize * diskInfo.f_bfree;
    return ret;
}



