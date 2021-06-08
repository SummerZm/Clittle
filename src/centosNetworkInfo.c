#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <netinet/in.h>  /* For INET_ADDRSTRLEN*/
#include <arpa/inet.h>   /* For inet_ntop(...) */

#define BOOL int
#define TRUE 1
#define FALSE 0

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

int main(int argc, char** argv)
{
    char ip[36];
    memset(ip, 0, sizeof(ip));
    setIpByInterfacesName("ip", "192.168.1.167");
    printf("%s\n", ip);
    setNetworkInfoByFacesName("ip", "192.168.3.221", "192.168.3.221", "255.255.0.0", "192.168.3.221", "192.168.3.221");
    getIpByInterfaceName(ip, sizeof(ip), "enp2s0");
    return 0;
}

