#include"communication.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <error.h>
#include <net/route.h>

// Print message content in the data package.
void show_data(char *str,int len)
{
	int i;
	printf("data length is:%d ",str[message_length_pos]);
	printf("my private data is:");
	for(i=0;i<len;i++) {
    	printf("%x ",str[i+message_pos]);
  	}
	printf("\n\n");
	printf("即:");
	for(i=0;i<len;i++) {
		printf("%c",str[i+message_pos]);
	}
	printf("\n");
} 

 
// Add message content to data package.
void add_data(unsigned char* str1,char* str2)
{
    int i;
    char len=strlen(str2);
    str1[message_length_pos]=len;

    for(i=0;i<len;i++)
       sprintf(&str1[i+message_pos],"%c",str2[i]);

}

// Called by ead_netdev_proc()
char *get_name(char *name, char *p)  
{  
    while (isspace(*p))
		p++;  
  
    while (*p) {  
		if (isspace(*p))  break;  
		if (*p == ':') {    /* could be an alias */  
			char *dot = p; *dotname = name;  
	
			/* high priority is ++;
			 * 	*p++ equal to *(p++) ==> Get the value and then the pointer move to nest position;
			 * 	*++p equal to *(++p) ==> The pointer move to next position and then get value.
			 * 	(*p)++ Get value and then the value add 1;
			 */
			*name++ = *p++;
			while (isdigit(*p))
				*name++ = *p++;
				
			if (*p != ':') {    /* it wasn't, backup */  
        		p = dot;
				name = dotname;  
        	}  
        	
			if (*p == '\0')  
        		return NULL;  
        	p++;  
       		break;  
    	}  
    	*name++ = *p++;  
    }  
    *name++ = '\0';  
    return p;  
}  
  
/***********************************************************
 * read_netdev_proc - read net dev names form proc/net/dev 
 * @devname: where to store dev names, devname[num][len] 
 **********************************************************/  
int read_netdev_proc(void *devname, const int num, const int len)  
{  
	FILE *fh;  
	char buf[512];  
	int cnt = 0;  
	char *dev = (char *)devname;  
  
	if(devname == NULL || num < 1 || len < 4){  
		printf("read_netdev_proc: para error\n");  
		return -1;  
	}  
  
    memset(devname, 0, len * num);  
    fh = fopen(_PATH_PROCNET_DEV, "r");  
    if (!fh) {  
		fprintf(stderr, "Warning: cannot open %s (%s). Limited output.\n",  
		_PATH_PROCNET_DEV, strerror(errno));   
		return -1;  
    }  
  	
	fgets(buf, sizeof buf, fh); /* eat two line */  
    fgets(buf, sizeof buf, fh);  
  
    cnt = 0;  
    while (fgets(buf, sizeof buf, fh) && cnt < num) {
		char *s,name[IFNAMSIZ];
		s = get_name(name, buf);
		strncpy(devname, name, len);
		devname += len;  
		printf("get_name: %s\n", name);  
	}  
  
	if (ferror(fh)) {
		perror(_PATH_PROCNET_DEV);  
    }  
  
    fclose(fh);  
    return 0;  
}  
  
/*********************************************************
 * get_hwaddr - get netdevice mac addr  
 * @name: device name, e.g: eth0 
 * @hwaddr: where to save mac, 6 byte hwaddr[6] 
 * @return: 0 on success, -1 on failure 
 ********************************************************/  
int get_hwaddr(char *name, unsigned char *hwaddr)  
{  
    struct ifreq ifr;  
    unsigned char memzero[6];  
    int sock;  
  
    if(name == NULL || hwaddr == NULL){  
        printf("get_hwaddr: NULL para\n");  
        return -1;  
    }  
  
	sock = socket(AF_INET, SOCK_STREAM, 0);  
	if(sock < 0){  
    	printf("get_hwaddr: socket error\n");  
        return -1;  
    }  
  
    //get eth1 mac addr  
    memset(hwaddr, 0, 6);  
    memset(&ifr, 0, sizeof(ifr));  
    strncpy(ifr.ifr_name, name, 6);  
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0){  
    	perror("get_hwaddr ioctl:");  
    	close(sock);  
    	return -1;  
    } else {  
    	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, 6);  
    	printf("hwaddr: %2x : %2x : %2x : %2x : %2x : %2x\n", hwaddr[0], hwaddr[1],hwaddr[2], hwaddr[3],hwaddr[4], hwaddr[5]);  
    }  
  
    memset(memzero, 0, 6);  
    if(memcmp(memzero, hwaddr, 6) == 0){  
    	printf("no mac\n");  
    	return -1;  
	}  
  
    close(sock);  
    return 0;  
}  
  

// Print data into hex format.
void printhex(void *hex, int len, char *tag)  
{  
    int i;  
    unsigned char *p = (unsigned char *)hex;  
  
    if(len < 1)  
        return;  
  
    for(i = 0; i < len - 1; i++){  
        if(*p < 0x10)  
            printf("0%x%s", *p++, tag);  
        else  
            printf("%2x%s", *p++, tag);  
    }  
  
    if(*p < 0x10)  
        printf("0%x\n", *p++);  
    else  
        printf("%2x\n", *p++);  
}  
  


// Network package data compare.
int compare(unsigned char* str1,unsigned char* str2)
{
	int i;
	for(i=0;i<=6;i++){
	if(str1[i]==str2[i])
    	continue;
		return -1;
	}
	return 0;
}


#define ETH_NAME "eth0" 

/************************************************************* 
* Function: Get ip address. 
* Argument: Ip address buffer. 
* Return:	Ip address or null.
**************************************************************/  
char *get_ip(char *ip)  
{  
    int sock;  
    struct sockaddr_in sin;  
    struct ifreq ifr;  
    char *temp_ip = NULL;  
      
    sock = socket(AF_INET, SOCK_DGRAM, 0);  
    if (sock == -1)  
    {  
        perror("socket");  
        return NULL;                  
    }  
    strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ-1);  
    ifr.ifr_name[IFNAMSIZ - 1] = 0;  
      
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)  
    {  
        perror("ioctl");  
        return NULL;  
    }  
  
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
    temp_ip = inet_ntoa(sin.sin_addr);  
    strcpy(ip,temp_ip);
    printf("local ip: %s\n", temp_ip);  
  
    return ip;  
}  

/************************************************************* 
* Function: Set ip addrss by hand. 
* Return: succss 0, fail -1. 
**************************************************************/  
int set_hand_ip(const char *ipaddr)  
{  
    int sock_set_ip;       
    struct sockaddr_in sin_set_ip;       
    struct ifreq ifr_set_ip;       
  
    memset( &ifr_set_ip,0,sizeof(ifr_set_ip));       
    if( ipaddr == NULL ) {  
        return -1;       
    }  
     
    sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 );  
    if(sock_set_ip<0)    
    {       
        perror("socket create failse...SetLocalIp!");       
        return -1;       
    }       
    memset( &sin_set_ip, 0, sizeof(sin_set_ip));       
    strncpy(ifr_set_ip.ifr_name, ETH_NAME, sizeof(ifr_set_ip.ifr_name)-1);          
  
    sin_set_ip.sin_family = AF_INET;       
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);       
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));       
    printf("ipaddr===%s\n",ipaddr);  
    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 ) {       
        perror( "Not setup interface");       
        return -1;       
    }       
  
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;        
  
    //get the status of the device        
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 ) {       
         perror("SIOCSIFFLAGS");       
         return -1;       
    }       
      
    close( sock_set_ip );       
    return 0;  
}  

int GetLocalNetMask(char* netmask_addr)  
{  
    if( netmask_addr == NULL )       
    {  
        return -1;       
    }
    int sock_netmask;
  
    struct ifreq ifr_mask;  
    struct sockaddr_in *net_mask;  
          
    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sock_netmask == -1)  
    {  
        perror("create socket failture...GetLocalNetMask\n");  
        return -1;  
    }  
      
    memset(&ifr_mask, 0, sizeof(ifr_mask));     
    strncpy(ifr_mask.ifr_name, ETH_NAME, sizeof(ifr_mask.ifr_name )-1);     
  
    if( (ioctl( sock_netmask, SIOCGIFNETMASK, &ifr_mask ) ) < 0 )   
    {  
        printf("mac ioctl error\n");  
        return -1;  
    }  
      
    net_mask = ( struct sockaddr_in * )&( ifr_mask.ifr_netmask );  
    strcpy( netmask_addr, inet_ntoa( net_mask -> sin_addr ) );  
      
    printf("local netmask:%s\n",netmask_addr);      
      
    close( sock_netmask );  
    return 0;  
}  

int GetGateWay(char* gateway)  
{  
    FILE *fp;  
    char buf[512];  
    char cmd[128];
    char *tmp;  
  
    strcpy(cmd, "ip route");  
    fp = popen(cmd, "r");  
    if(NULL == fp)  
    {  
        perror("popen error");  
        return -1;  
    }  
    while(fgets(buf, sizeof(buf), fp) != NULL)  
    {  
        tmp =buf;  
        while(*tmp && isspace(*tmp))  
            ++ tmp;  
        if(strncmp(tmp, "default", strlen("default")) == 0)  
            break;  
    }  
    sscanf(buf, "%*s%*s%s", gateway);         
    printf("default gateway:%s\n", gateway);  
    pclose(fp);  
      
    return 0;  
}  

int SetIfAddr(char *ifname, char *Ipaddr)
{
    int fd;
    int rc;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
    struct rtentry  rt;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
		perror("socket   error");     
		return -1;     
    }
    memset(&ifr,0,sizeof(ifr)); 
    strcpy(ifr.ifr_name,ifname); 
    sin = (struct sockaddr_in*)&ifr.ifr_addr;     
    sin->sin_family = AF_INET;  
   
    //ipaddr
    if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0) {     
        perror("inet_aton   error");     
        return -2;     
    }    
    
    if(ioctl(fd,SIOCSIFADDR,&ifr) < 0) {     
        perror("ioctl   SIOCSIFADDR   error");     
        return -3;     
    }
    return 0;
}

int check_ip_format(char* IpStr)
{
	int a, b, c, d, ret = -1;
	if(IpStr != NULL) {
		const char* format = "%d.%d.%d.%d";
		if(sscanf(IpStr, format, &a, &b, &c, &d)==4 && a>=0 && a<=255 &&
			b>=0 && b<=255 && c>=0 && c<=255 && d>=0 && d<=255) {
			ret = 1;
		}
	}
	return ret;
}




