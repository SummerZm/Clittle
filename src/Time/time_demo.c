#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

#define BUFFLEN 255
int main(int argc, char** argv){
	struct timeval tv;
	time_t t=time(0);
	struct tm tm; 
	char tmpBuf[BUFFLEN];
	gettimeofday(&tv, NULL);
	printf("获取当前系统时间\n");
	printf("second: %lu\n", tv.tv_sec);
	printf("millisecond(毫秒): %lu\n", tv.tv_sec+tv.tv_usec/1000);
	printf("microsecond(微秒): %lu\n", tv.tv_sec*1000000+tv.tv_usec);
	printf("\n格式化输出当前系统时间\n");
	strftime(tmpBuf, BUFFLEN, "%Y-%m-%d %T", localtime(&t));
	//strftime(tmpBuf, BUFFLEN, "%Y%m%d%H%M%S", localtime(&t));
	printf("当前时间(sec)转字符串:\n%s\n", tmpBuf);


    strptime(tmpBuf, "%Y-%m-%d %T" , &tm);
    memset(tmpBuf,0,sizeof(tmpBuf));
    strftime(tmpBuf, sizeof(tmpBuf), "%s", &tm);
	printf("\n时间字符串转秒数:\n");
	printf("\n设置不同的format,可以输出不同的时间格式\n");
    puts(tmpBuf);

//==========================================================================================
    
    printf("\n获取系统运行时间\n");
    struct sysinfo info;
    sysinfo(&info);
    printf("系统运行时间: %ds\n", info.uptime);
    unsigned int seconds = info.uptime;
    int day = seconds/86400;
    seconds %= 86400;
    int hour = seconds/3600;
    seconds %= 3600;
    int minute = seconds/60;
    int second = seconds%60;
    printf("系统运行时间: %d days %02d:%02d:%02d\n", day, hour, minute, second);
    //printf("系统运行时间ctime: %s", ctime((time_t)info.uptime));

    printf("\n获取系统启动时间\n");
    time_t boot_time = time(NULL) - info.uptime;
    struct tm *p = localtime(&boot_time);       // 静态变量无需释放,线程不安全
    printf("%d-%d-%d %d:%d:%d\n", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday,
        p->tm_hour, p->tm_min, p->tm_sec); 

//==========================================================================================
    printf("\n时间精度\n");
    printf("time(0): 秒\n");
    printf("_ftime(0): 毫秒\n");
    printf("gettimeofday(struct timeval*, NULL): 微秒\n");
    printf("clock_gettime(xx,struct spec*) : 纳秒\n");
    
    int64_t currTime;
    struct timespec spec;
    // CLOCK_REALTIME  -- settime()/settimeofday() calls and can also be frequency corrected by NTP via adjtimex(). 
    // CLOCK_MONOTONIC -- It is not affected by settime()/settimeofday(), but is frequency adjusted by NTP via adjtimex().
    // CLOCK_MONOTONIC_RAW -- that will not be modified at all, and will have a linear correlation with the hardware counters. 
    // CLOCK_MONOTONIC_RAW -- 记录系统启动时间严格单调递增，不受NTP影响
    // CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间  
    // CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间
    /********************************************************************************************************************* 
     *  With Linux,NTP normally uses settimeofday() for large corrections (over half a second). 
     *  The adjtimex() inteface allows for small clock frequency changes (slewing).
     *  This can be done in a few different ways, see the man page for adjtimex clock_gettime(CLOCK_MONOTONIC, &spec);
     ********************************************************************************************************************/
    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);    
    currTime = (int64_t)spec.tv_sec*1000000000 + spec.tv_nsec;
    printf("\n系统运行时间clock_gettime: %llu ns\n", currTime);

    //clock_gettime(CLOCK_MONOTONIC, &spec);    
    //currTime = (int64_t)spec.tv_sec*1000000000 + spec.tv_nsec;
    //printf("\n系统运行时间clock_gettime: %llu ns\n", currTime);
    /******************************************************************************************************************************************
     * wall time：xtime取决于用于对xtime计时的clocksource，它的精度达到纳秒级，内核大部分时间都是使用xtime来获得当前时间信息。
     * xtime: 记录的是自1970年当前时刻所经历的纳秒数。(可以因用户的调整时间而产生跳变) 
     * monotonic time: 该时间自系统开机后就一直单调地增加（ntp adjtimex会影响其单调性），系统休眠时（total_sleep_time)，monotoic时间不会递增。 
     * raw monotonic time: 单调递增的时间， 但raw monotonic time不会受到NTP时间调整的影响，它代表着系统独立时钟硬件对时间的统计。 
     * boot time: 与monotonic时间相同，不过会累加上系统休眠的时间(total_sleep_time)，它代表着系统上电后的总时间。 
     ******************************************************************************************************************************************/

    //在linux下获取高精度单调递增的时间
    //syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW, &spec);
	
	// 设置时间的两种方式
	struct timeval tv1;
	struct tm tm1; 
	char* tmBuf= "2029-10-10 12:00:00";
    strptime(tmBuf, "%Y-%m-%d %T" , &tm1);
	time_t time_temp = mktime(&tm1);
	//stime(&time_temp);
	tv.tv_sec = time_temp;
	tv.tv_usec = 0;
	//settimeofday(&tv, NULL);
    return 0;
}


    


