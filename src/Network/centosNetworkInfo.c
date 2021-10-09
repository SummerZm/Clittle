#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>  /* For INET_ADDRSTRLEN*/
#include <arpa/inet.h>   /* For inet_ntop(...) */

#define BOOL int
#define TRUE 1
#define FALSE 0

int executeCMD(const char *cmd, char* buff, int len) {
	int rbytes=0, total=len, ret=-1;
	FILE *pfd=NULL;
	if(cmd==NULL || buff==NULL || len<=0) {
		return ret;
	}

	memset(buff, 0, len);
	if( (pfd=popen(cmd, "r")) != NULL) {
		while (len>sizeof(char)) {
			errno = 0;
			rbytes=fread(buff+total-len, sizeof(char), len, pfd);
			// printf("cmd[%s] \ndata [%s] len:[%d] rbytes:[%d] \nerrno[%d]", cmd, buff, len, rbytes, errno);
			if (rbytes==0) {
				if (errno!=0) {
            		printf("cmd %s failed %s", cmd, strerror(errno));
					ret = -1;
					break;
				}
				else {
					ret = strlen(buff);
					break;
				}
			}
			len -= rbytes;
		}

		if(pclose(pfd)==-1) {
            printf("cmd %s failed %s", cmd, strerror(errno));
        };
	}
    else {
        printf("cmd %s failed %s", cmd, strerror(errno));
    }
	return ret;
}

/* Output: interfacename1|interfacename2|... */
void getInferfacesName(char *nameBuff, int len)
{
	int avalible = len;
	struct ifaddrs *ifAddrStruct_list, *ifAddrStruct = NULL;
	if (len<=0 || nameBuff==NULL)
	{
		printf("getInferfacesName len=0\n");
		return;
	}
	getifaddrs(&ifAddrStruct_list);
	ifAddrStruct = ifAddrStruct_list;
	while (ifAddrStruct != NULL)
	{
		int ifa_nameLen = 0;
		if (ifAddrStruct->ifa_name == NULL) 
		{
			printf("ifa_name is null! skip.");
			ifAddrStruct=ifAddrStruct->ifa_next;
			continue;
		}
		if (ifAddrStruct->ifa_addr->sa_family != AF_INET) 
		{
			ifAddrStruct=ifAddrStruct->ifa_next;
			continue;
		}
		ifa_nameLen = strlen(ifAddrStruct->ifa_name);
		if (avalible <= ifa_nameLen+1)
		{
			printf("Buffer is no enough! exit");
			break;
		}
		sprintf(nameBuff+len-avalible, "%s|", ifAddrStruct->ifa_name);
		avalible -= ifa_nameLen+1;
		ifAddrStruct=ifAddrStruct->ifa_next;
	}
	freeifaddrs(ifAddrStruct_list);
	*(nameBuff+len-avalible-1) = '\0';
	return;
}

/* Just for centos. */
BOOL updateNetworkInfoByFacesName(char* interfaceName, char* networkInfoKey,  char* networkInfoVal)
{
	char cmd[256];
	char interfaceFile[256];
	char* interfacePath = "/etc/sysconfig/network-scripts/ifcfg-";
	char* cmdTemplate =  "sed -i '/%s\=/c\%s=%s' %s";
	memset(cmd, 0, sizeof(cmd));
	memset(interfaceFile, 0, sizeof(interfaceFile));

	if (interfaceName==NULL || networkInfoVal==NULL || networkInfoKey==NULL) 
	{
		return FALSE;
	}
	sprintf(interfaceFile, "%s%s", interfacePath, interfaceName);
	sprintf(cmd, cmdTemplate, networkInfoKey, networkInfoKey, networkInfoVal, interfaceFile);
	printf("%s\n", cmd);
	system(cmd);
	return TRUE;
}

BOOL getNetworkMaskPrerfix(char* networkmask, char* strPrefix, int len) 
{
	char netmask[20];
	int prefix = 0;
	BOOL bret = FALSE;
	char *end, *start;
	if (networkmask==NULL || strPrefix==NULL || len<=0)
	{
		return bret;
	}
	
	start = netmask;
	memset(netmask, 0, sizeof(netmask));
	memset(strPrefix, 0, len);
	strcpy(netmask, networkmask);
	end = strstr(start, "0");
	if (end!=NULL) 
	{
		*end = '\0';
	}

	while ((end=strstr(start,".")) != NULL) 
	{
		*end = '\0';
		if (strcmp("255", start)==0)
		{
			prefix += 8;
		}
		else if (strcmp("127", start)==0) 
		{
			prefix += 7;
			break;
		}
		else if (strcmp("63", start)==0) 
		{
			prefix += 6;
			break;
		}
		else if (strcmp("31", start)==0) 
		{
			prefix += 5;
			break;
		}
		else if (strcmp("15", start)==0) 
		{
			prefix += 4;
			break;
		}
		else if (strcmp("7", start)==0) 
		{
			prefix += 3;
			break;
		}
		else if (strcmp("3", start)==0) 
		{
			prefix += 2;
			break;
		}
		else if (strcmp("1", start)==0) 
		{
			prefix += 1;
			break;
		}
		start = end+1;
	}
	sprintf(strPrefix, "%d", prefix);
	return TRUE;
}

/* Just for centos. */
int setNetworkInfoByFacesName(char* interfaceName, char* ip, char* gateway, char* networkmask, char* dns1, char* dns2)
{
	if (interfaceName==NULL)
	{
		return FALSE;
	}
	if (ip!=NULL) 
	{
		updateNetworkInfoByFacesName(interfaceName, "IPADDR", ip);
	}
	if (gateway!=NULL) 
	{
		updateNetworkInfoByFacesName(interfaceName, "GATEWAY", gateway);
	}
	if (dns1!=NULL) 
	{
		updateNetworkInfoByFacesName(interfaceName, "DNS1", dns1);
	}
	if (dns2!=NULL) 
	{
		updateNetworkInfoByFacesName(interfaceName, "DNS2", dns2);
	}
	if (networkmask!=NULL) 
	{
		char prefix[4];
		memset(prefix, 0, sizeof(prefix));
		if (TRUE == getNetworkMaskPrerfix(networkmask, prefix, sizeof(prefix))) 
		{
			updateNetworkInfoByFacesName(interfaceName, "PREFIX", prefix);
		}
	}
	return 0;
}

int getIpByInterfaceName(char* ip, int len, char* ifName) {
    int ret=0, targetIfNamelen=0;
	struct ifaddrs *ifList, *ifListHead;
    if (ip==NULL || ifName==NULL || len<INET_ADDRSTRLEN) {
        //printf("Invalid input.\n");
        return ret;
    }

    targetIfNamelen = strlen(ifName);
    getifaddrs(&ifList);
    ifListHead = ifList;
    while (ifList != NULL) {
        int ifaNameLen = 0;
        if (ifList->ifa_addr->sa_family!=AF_INET) {
            ifList = ifList->ifa_next;
            continue;
        }
       
        ifaNameLen = strlen(ifList->ifa_name);
        if (strncmp(ifName, ifList->ifa_name, targetIfNamelen)==0 
            && targetIfNamelen==ifaNameLen) {
			inet_ntop(AF_INET, &((struct sockaddr_in *)ifList->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
            //printf("Ip:%s\n", ip);
        }
        ifList = ifList->ifa_next;
    }
	freeifaddrs(ifListHead);
    return ret;
}


/* Just for centos. */
int setIpByInterfacesName(char* interfaceName, char* ip)
{
	char cmd[256];
	char interfaceFile[256];
	char* interfacePath = "/etc/sysconfig/network-scripts/ifcfg-";
	char* cmdTemplate =  "sed -i '/IPADDR\=/c\IPADDR=%s' %s";
	memset(cmd, 0, sizeof(cmd));
	memset(interfaceFile, 0, sizeof(interfaceFile));

	if (interfaceName==NULL || ip==NULL) 
	{
		return 0;
	}
	sprintf(interfaceFile, "%s%s", interfacePath, interfaceName);
	sprintf(cmd, cmdTemplate, ip, interfaceFile);
	printf("%s\n", cmd);
	system(cmd);
	return 1;
}


BOOL getNetworkInfoByFacesName(char* interfaceName, char* networkInfoKey,  char* networkInfoVal, int len)
{
    char cmd[256];
    char interfaceFile[256];
	char* interfacePath = "/etc/sysconfig/network-scripts/ifcfg-";
	char* cmdTemplate = "cat %s | grep %s | awk 'BEGIN{FS=\"=\"}{print $2}'";
	memset(cmd, 0, sizeof(cmd));
	memset(interfaceFile, 0, sizeof(interfaceFile));

	if (interfaceName==NULL || networkInfoVal==NULL || networkInfoKey==NULL) 
	{
		return FALSE;
	}
	sprintf(interfaceFile, "%s%s", interfacePath, interfaceName);
	sprintf(cmd, cmdTemplate, interfaceFile, networkInfoKey);
	printf("%s\n", cmd);
    executeCMD(cmd, networkInfoVal, len);
    *(networkInfoVal+strlen(networkInfoVal)-1) = '\0';
    printf("%s [%s]\n", networkInfoKey, networkInfoVal);
    return TRUE;
}

BOOL getNetworkMaskStr(char* strPrefix, char* netmaskStr, int len)
{
    int loop = 0;
    int prefix = 0;
    if (strPrefix==NULL || netmaskStr==NULL || len<=0)
    {
        return FALSE;
    }
    memset(netmaskStr, 0, len);
    prefix = atoi(strPrefix);
    while (prefix-8>=0) 
    {
        strcat(netmaskStr, "255.");
        prefix-=8;
        loop++;
        if (prefix==0) break;
    }
    if (loop<4) 
    {
        switch(prefix)
        {
            case 0:
                strcat(netmaskStr, "0.");
                break;
            case 1:
                strcat(netmaskStr, "1.");
                break;
            case 2:
                strcat(netmaskStr, "3.");
                break;
            case 3:
                strcat(netmaskStr, "7.");
                break;
            case 4:
                strcat(netmaskStr, "15.");
                break;
            case 5:
                strcat(netmaskStr, "31.");
                break;
            case 6:
                strcat(netmaskStr, "63.");
                break;
            case 7:
                strcat(netmaskStr, "127.");
                break;
        }
        loop++;
        while (loop<4) 
        {   
            strcat(netmaskStr, "0.");
            loop++;
        }
        *(netmaskStr+strlen(netmaskStr)-1) = '\0';
    }
    printf("network mask: %s\n", netmaskStr);
    return TRUE;
}

void Test_getNetworkInfoByFacesName()
{
 //BOOL getNetworkInfoByFacesName(char* interfaceName, char* networkInfoKey,  char* networkInfoVal, int len)
   char networkInfoVal[64];
   char networkmask[64];
   memset(networkInfoVal, 0, sizeof(networkInfoVal));
   getNetworkInfoByFacesName("ip", "IPADDR", networkInfoVal, sizeof(networkInfoVal));
   getNetworkInfoByFacesName("ip", "DNS1", networkInfoVal, sizeof(networkInfoVal));
   getNetworkInfoByFacesName("ip", "DNS2", networkInfoVal, sizeof(networkInfoVal));
   getNetworkInfoByFacesName("ip", "PREFIX1", networkInfoVal, sizeof(networkInfoVal));
   getNetworkMaskStr(networkInfoVal, networkmask, sizeof(networkmask));
   getNetworkInfoByFacesName("ip", "GATEWAY", networkInfoVal, sizeof(networkInfoVal));
}

int main(int argc, char** argv)
{
    char ip[36];
    memset(ip, 0, sizeof(ip));
    setIpByInterfacesName("ip", "192.168.1.167");
    printf("%s\n", ip);
    setNetworkInfoByFacesName("ip", "192.168.3.221", "192.168.3.221", "255.255.0.0", "192.168.3.221", "192.168.3.221");
    getIpByInterfaceName(ip, sizeof(ip), "enp2s0");
	Test_getNetworkInfoByFacesName();
    return 0;
}

