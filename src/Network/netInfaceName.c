#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <netinet/in.h>  /* For INET_ADDRSTRLEN*/
#include <arpa/inet.h>   /* For inet_ntop(...) */

void printfInterfaceName() {
	struct ifaddrs *ifList, *ifListHead;
    getifaddrs(&ifList);
    ifListHead = ifList;
    while (ifList != NULL) {
        printf("Interfaces: %15s   NetWork Family: %d\n", ifList->ifa_name, ifList->ifa_addr->sa_family);
        ifList = ifList->ifa_next;
    }
	freeifaddrs(ifListHead);
}

int getIpByInterfaceName(char* ip, int len, char* ifName) {
    int ret=0, targetIfNamelen=0;
	struct ifaddrs *ifList, *ifListHead;
    if (ip==NULL || ifName==NULL || len<INET_ADDRSTRLEN) {
        printf("Invalid input.\n");
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
            printf("Ip:%s\n", ip);
        }
        ifList = ifList->ifa_next;
    }
	freeifaddrs(ifListHead);
    return ret;
}

int getInterfaceByIndex(char* ifName, int len, int index) {
    int ret = 0;
	struct ifaddrs *ifList, *ifListHead;
    if (ifName==NULL || len<=0 || index<0) {
        printf("Invalid input.\n");
        return ret;
    }

    getifaddrs(&ifList);
    ifListHead = ifList;
    while (ifList != NULL && index>=0) {
        if (ifList->ifa_addr->sa_family==AF_INET) {
            continue;
        }

        if (index==0) {
            int ifNameLen = strlen(ifList->ifa_name);
            if (len>ifNameLen) {
                memset(ifName, 0, len);
                strncpy(ifName, ifList->ifa_name, ifNameLen);
                ret = 1;
            }
            else {
                printf("Too small: Interface name len %d, Buffer len %d.\n", len, ifNameLen);
            }
            break;
        }
        index--;
        ifList = ifList->ifa_next;
    }
	freeifaddrs(ifListHead);
    return ret;
}

/* Output: interfacename1|interfacename2 */
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
			printf("Buffer is no enough! exit\n");
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
   
int main(int argc, char** argv) {
    printfInterfaceName();
    char ifName[10];
    char ip[INET_ADDRSTRLEN];
    memset(ip, 0, INET_ADDRSTRLEN);
    getInterfaceByIndex(ifName, sizeof(ifName), 0);
    getIpByInterfaceName(ip, sizeof(ip), ifName);
    printf(">>>Ifname: %s Ip: %s<<<\n", ifName, ip);
    return 0;
}
