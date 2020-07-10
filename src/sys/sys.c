/*************************************************************************
 > File Name: sys.c
 > Author: Leafxu
 > Created Time: 2020年07月08日 星期三 13时37分28秒
 ************************************************************************/
#include <stdio.h>
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

unsigned long getTotalMem() {
    struct sysinfo info;
    if (sysinfo(&info)) {
        return -1;
    }
    return info.totalram;
}

unsigned long getFreeMem() {
    struct sysinfo info;
    if (sysinfo(&info)) {
        return -1;
    }
    return info.freeram;
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

/*
 * cpu  426215 701 115732 2023866 27329 4 557 0 0 0 （单位：jiffies）
 * user(426215)   用户态的CPU时间，不包含 nice值为负进程
 * nice(701)      nice值为负的进程所占用的CPU时间
 * system(115732) kernel时间
 * idle(2023866)  除硬盘IO等待时间以外其它等待时间
 * iowait(27329)  硬盘IO等待时间
 * irq(4)         硬中断时间
 * softirq(557)   软中断时间
 */
typedef struct occupy {
	char name[20];
	float user;
	float nice;
	float system;
	float idle;
} CPU_OCCUPY;

int get_occupy(CPU_OCCUPY *o){
	FILE *fd;
	fd = fopen("/proc/stat", "r");
	if (fd == NULL){
		return -1;
	}
	fscanf(fd, "%s %f %f %f %f", o->name, &o->user, &o->nice, &o->system, &o->idle);
	fclose(fd);
    return 0;
}

void cal_occupy(CPU_OCCUPY* o, CPU_OCCUPY* n, char* cpu_used){
	float total, idle;
	idle = n->idle - o->idle;
	total = n->user + n->nice + n->system + n->idle - (o->user + o->nice + o->system + o->idle);
    sprintf(cpu_used, "%4.2f%s", 100.0 * (total - idle) / total, "%");
}

void getCpuUsageRate(const char* buff) {
	int sleep_time = 400;
	char cpu_used[12];
	CPU_OCCUPY ocpu, ncpu;
	memset(cpu_used, sizeof(cpu_used), 0);

	get_occupy(&ocpu);
	sleep_ms(sleep_time);
	get_occupy(&ncpu);

	cal_occupy(&ocpu, &ncpu, &cpu_used);
	sprintf(buff, "%s", cpu_used);
	return;
}









