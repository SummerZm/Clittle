/*
#include <sys/ioctl.h>
int ioctl(int fd, int request, … * void *arg *);
返回：成功返回0，失败返回-1
char *inet_ntoa (struct in_addr);
　　返回点分十进制的字符串在静态内存中的指针。
　　所在头文件：<arpa/inet.h>　函数功能：将网络地址转换成“.”点隔的字符串格式。
　　所需库： winsock.h
#include<linux.h>
struct ifconf 
{
	int	ifc_len;			/* size of buffer	*
	union 
	{
		char __user *ifcu_buf;
		struct ifreq __user *ifcu_req;
	} ifc_ifcu;
};
struct ifreq 
{
#define IFHWADDRLEN	6
	union
	{
		char	ifrn_name[IFNAMSIZ];		* if name, e.g. "en0" *
	} ifr_ifrn;
	
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		struct	sockaddr ifru_netmask;
		struct  sockaddr ifru_hwaddr;
		short	ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct  ifmap ifru_map;
		char	ifru_slave[IFNAMSIZ];	/* Just fits the size *
		char	ifru_newname[IFNAMSIZ];
		void __user *	ifru_data;
		struct	if_settings ifru_settings;
	} ifr_ifru;
};
#define ifr_name	ifr_ifrn.ifrn_name	/* interface name 	*
#define ifr_hwaddr	ifr_ifru.ifru_hwaddr	/* MAC address 		*
#define	ifr_addr	ifr_ifru.ifru_addr	/* address		*
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-p lnk	*
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address	*
#define	ifr_netmask	ifr_ifru.ifru_netmask	/* interface net mask	*
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags		*
#define	ifr_metric	ifr_ifru.ifru_ivalue	/* metric		*
#define	ifr_mtu		ifr_ifru.ifru_mtu	/* mtu			*
#define ifr_map		ifr_ifru.ifru_map	/* device map		
#define ifr_slave	ifr_ifru.ifru_slave	/* slave device		*
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface	*
#define ifr_ifindex	ifr_ifru.ifru_ivalue	/* interface index	
#define ifr_bandwidth	ifr_ifru.ifru_ivalue    /* link bandwidth	*
#define ifr_qlen	ifr_ifru.ifru_ivalue	/* Queue length 	*
#define ifr_newname	ifr_ifru.ifru_newname	/* New name		*
#define ifr_settings	ifr_ifru.ifru_settings	/* Device/proto settings*
*/
#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 
#include <arpa/inet.h>
#ifdef SOLARIS 
#include <sys/sockio.h> 
#endif 
#define MAXINTERFACES 16 /* 最大接口数 */

typedef enum __InterfaceProp
{
	NETINFO_STATUS = 1,
	NETINFO_IPADDR,
 	NETINFO_NETMASK,
 	NETINFO_MAC,
    NETINFO_BROADCAST
} InterfaceProp;

/*
 * @interfaceName: network adapter name.
 * @interfacePropKey: ip | mac | network mask | gateway.
 * @interfacePropVal: output buffer.
 * @len: the length of buffer.
 */
int getNetworkinfoByInterfaceName(const char* interfaceName, InterfaceProp interfacePropKey, char* interfacePropVal ,int len )
{
	struct arpreq arp; 
	struct ifconf ifc; 
	struct ifreq buf[MAXINTERFACES];  
	int interfaceCnt, fd, target=0, retLen=-1;
	if (interfaceName==NULL || interfacePropKey==NULL || interfacePropVal==NULL || len<=0)
	{	
		return retLen;
	}

	memset(interfacePropVal, 0, len);
	fd = socket (AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		return retLen;
	}
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	/* Get all interfaces list. */
	if (ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
		printf("Get interface list failed\n");
		goto end;
	}
	interfaceCnt = ifc.ifc_len / sizeof(struct ifreq);
	//printf("interface num is intrface=%d\n", interfaceCnt); 

    /* Get target interface. */
    for (target=0; target<interfaceCnt; target++) {
        if (strcmp(buf[target].ifr_name, interfaceName)==0) {
            break;
        }
    }
    if (target>=interfaceCnt) {
        printf("Interface is't exist.\n");
        goto end;
    }

    switch (interfacePropKey)
    {
        case NETINFO_STATUS: 
        {
            if (len < 5) {
                printf("interfacePropVal buffer is too smaall\n");
                break;
            }
            if (buf[target].ifr_flags & IFF_UP) {
                strcpy(interfacePropVal, "UP");
            }
            else {
                strcpy(interfacePropVal, "DOWN");
            }
            break;
        }
        case NETINFO_IPADDR:
        {
            if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[target])))
            {
                const char* ipaddr = inet_ntoa(((struct sockaddr_in*)(&buf[target].ifr_addr))->sin_addr);
                if (ipaddr && strlen(ipaddr)<len)
                {
                    strcpy(interfacePropVal, ipaddr);
                }
            } 
            break;
        }
        case NETINFO_NETMASK:
        {
            if (!(ioctl(fd, SIOCGIFNETMASK, (char *) &buf[target]))) 
            {
                const char* netmask = inet_ntoa(((struct sockaddr_in*) (&buf[target].ifr_addr))->sin_addr);
                if (netmask && strlen(netmask)<len)
                {
                    strcpy(interfacePropVal, netmask);
                }
            }
            break;
        }
        case NETINFO_MAC:
        {
            if (!ioctl(fd, SIOCGIFHWADDR, (char*)&buf[target]) && len>18) {
                sprintf(interfacePropVal, "%02x:%02x:%02x:%02x:%02x:%02x",
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[0], 
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[1], 
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[2], 
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[3], 
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[4], 
                    (unsigned char)buf[target].ifr_hwaddr.sa_data[5]); 
            }
            break;
        }
        case NETINFO_BROADCAST:
        {
            if (!(ioctl(fd, SIOCGIFBRDADDR, (char *) &buf[target]))) 
            {
                const char* broadcast = inet_ntoa(((struct sockaddr_in*) (&buf[target].ifr_addr))->sin_addr);
                if (broadcast && strlen(broadcast)<len)
                {
                    strcpy(interfacePropVal, broadcast);
                }
            }
            break;
        }
	}
    printf("%s:[%d]:%s\n", interfaceName, interfacePropKey, interfacePropVal);
	//return strlen(interfacePropVal);
end:
	close(fd);
	return strlen(interfacePropVal);
}

int  getNetworkInfo()
{ 
	register int fd, intrface, retn = 0; 
	struct ifreq buf[MAXINTERFACES]; /* ifreq结构数组 */
	struct arpreq arp; 
	struct ifconf ifc; 
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{ 
		ifc.ifc_len = sizeof buf; 
		ifc.ifc_buf = (caddr_t) buf; 
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{ 
			//获取接口数量信息
			intrface = ifc.ifc_len / sizeof (struct ifreq); 
			printf("interface num is intrface=%d\n",intrface); 
			//根据借口信息循环获取设备IP和MAC地址
			while ( (intrface--) > 0) 
			{ 
				//获取设备名称
				printf ("net device %s\n", buf[intrface].ifr_name); 
 
				//判断网卡类型 
				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) 
				{ 
					if (buf[intrface].ifr_flags & IFF_PROMISC) 
					{ 
						puts ("the interface is PROMISC"); 
						retn++; 
					} 
				} 
				else 
				{ 
					char str[256]; 
					sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					perror (str); 
				} 
				//判断网卡状态 
				if (buf[intrface].ifr_flags & IFF_UP) 
				{ 
					puts("the interface status is UP"); 
				} 
				else 
				{ 
					puts("the interface status is DOWN"); 
				} 
				//获取当前网卡的IP地址 
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface]))) 
				{ 
					printf("IP address is: %s\n", (char *)inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr)); 
				} 
				else 
				{ 
					char str[256]; 
					sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					perror (str); 
				} 
				/* this section can't get Hardware Address,I don't know whether the reason is module driver*/ 
#ifdef SOLARIS 
				//获取MAC地址
				arp.arp_pa.sa_family = AF_INET; 
				arp.arp_ha.sa_family = AF_INET; 
				((struct sockaddr_in*)&arp.arp_pa)->sin_addr.s_addr=((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr.s_addr; 
				if (!(ioctl (fd, SIOCGARP, (char *) &arp))) 
				{ 
					printf("HW address is:"); 
					//以十六进制显示MAC地址
					printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
							(unsigned char)arp.arp_ha.sa_data[0], 
							(unsigned char)arp.arp_ha.sa_data[1], 
							(unsigned char)arp.arp_ha.sa_data[2], 
							(unsigned char)arp.arp_ha.sa_data[3], 
							(unsigned char)arp.arp_ha.sa_data[4], 
							(unsigned char)arp.arp_ha.sa_data[5]); 
					puts(""); 
					puts(""); 
				} 
#else 
				if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &buf[intrface]))) 
				{ 
					printf("HW address is:"); 
					printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[0], 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[1], 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[2], 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[3], 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[4], 
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]); 
				} 
#endif 
				else 
				{ 
					char str[256]; 
					sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					perror (str); 
				} 
				//子网掩码
				if (!(ioctl(fd, SIOCGIFNETMASK, (char *) &buf[intrface])))
				{
					printf("MASK:%s",
						(char*)inet_ntoa(((struct sockaddr_in*) (&buf[intrface].ifr_addr))->sin_addr));
					puts(""); 
				}
			    else
				{
					char str[256];
					sprintf(str, "SIOCGIFADDR ioctl %s", buf[intrface].ifr_name);
					perror(str);
			   	}
			   	//广播地址
			   	if (! (ioctl(fd, SIOCGIFBRDADDR, (char *) &buf[intrface])))
					printf("Broadcast Address:%s\n",
						(char*)inet_ntoa(((struct sockaddr_in*) (&buf[intrface].ifr_addr))->sin_addr));
					puts(""); 
					puts(""); 
				} //while
			} 
			else {
				perror ("cpm: ioctl"); 
			}
		} 
		else {
			perror ("cpm: socket"); 
		}
	close (fd); 
	return retn; 
}

int main(int argc, char** argv)
{
    char netInfoBuff[24];
    getNetworkInfo();
    getNetworkinfoByInterfaceName("docker0", NETINFO_MAC, netInfoBuff, sizeof(netInfoBuff));
    getNetworkinfoByInterfaceName("docker0", NETINFO_IPADDR,netInfoBuff,  sizeof(netInfoBuff));
    getNetworkinfoByInterfaceName("docker0", NETINFO_NETMASK, netInfoBuff, sizeof(netInfoBuff));
    getNetworkinfoByInterfaceName("docker0", NETINFO_STATUS, netInfoBuff, sizeof(netInfoBuff));
    getNetworkinfoByInterfaceName("docker0", NETINFO_BROADCAST, netInfoBuff, sizeof(netInfoBuff));
    return 0;
}








