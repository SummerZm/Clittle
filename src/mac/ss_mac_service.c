#include <stdio.h>
#include <ss_mac_service.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <sys/ioctl.h> 

unsigned char comparision[] = {
	0xff,0xff,0xff,0xff,0xff,0xff
};

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
  
int get_hwaddr(char *name, unsigned char *hwaddr)
{
	struct ifreq ifr;
	unsigned char memzero[6];
	int sock;
	
	if(name == NULL || hwaddr == NULL) {
		printf("get_hwaddr: NULL para\n");
		return -1;
	}
	
	sock = socket(AF_INET, SOCK_STREAM,	0);
	if(sock < 0) {
		printf("get_hwaddr: socker error\n");
	}

	memset(hwaddr, 0, 6);  
	memset(&ifr, 0, sizeof(ifr));  
	strncpy(ifr.ifr_name, name, 6);  
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0){  
		perror("get_hwaddr ioctl:");  
		close(sock);  
		return -1;  
	} 
	else {  
		memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, 6);  
		printf("hwaddr: %2x : %2x : %2x : %2x : %2x : %2x\n", hwaddr[0], hwaddr[1],hwaddr[2], hwaddr[3],hwaddr[4], hwaddr[5]);  
	}

	memset(memzero, 0, 6);
	if(memcmp(memzero, hwaddr, 6) == 0) {
		printf("no mac\n");
		return -1;
	}

	close(sock);
	return 0;
}

void add_data(unsigned char* str1,char* str2)
{
    int i;
    char len=strlen(str2);
    str1[message_length_pos]=len;

    for(i=0;i<len;i++)
       sprintf(&str1[i+message_pos],"%c",str2[i]);

}

int send_mac_message(int sock, char* frommac, char* hwaddr, char* FCS, char* message) 
{	
	int length, ret = -1;
	unsigned char sendbuf[bufsize];
	char protocol[]={0x88, 0x8e};
	memset(sendbuf, 0, bufsize);
	memcpy(sendbuf, frommac, mac_size);  
	memcpy(sendbuf+mac_size, hwaddr, mac_size); 
	memcpy(sendbuf+fcs_pos,FCS, 2);
	memcpy(sendbuf+protocol_pos, protocol, 2);
	length = message_pos + strlen(message);	
	add_data(sendbuf,message);
		
	if(sock>0 && message!=NULL && length>0 ){
		struct sockaddr_ll sll;
		memset(&sll, 0, sizeof(sll));
		sll.sll_pkttype = PACKET_HOST;
		sll.sll_ifindex = 2;	// The index of local interface which send data.
		ret = sendto(sock, sendbuf, length, 0, (const struct sockaddr*)&sll, sizeof(sll));
	}
	printhex(sendbuf,ret," ");
	printf("send: %d\n", ret);
	return ret;
}


int handle_mac_message(int sock, char* receive_data, char* hwaddr)
{
	char message[message_size];	
	char frommac[mac_size];
	char respone_msg[message_size];

	if(receive_data == NULL || hwaddr == NULL) {
		printf("mac message content is null\n");
		return -1;
	}
	
	memset(frommac, 0, mac_size);
	memset(message, 0, message_size);
	memset(respone_msg, 0, message_size);

	memcpy(frommac, receive_data+mac_size, mac_size);
	memcpy(message, receive_data+message_pos, receive_data[message_length_pos]);
	
	if(strncmp(message, "ModifyNetInfo", strlen("ModifyNetInfo")) == 0) {
		char FCS1[]={0x22, 0x23};
		char tmp_token[14], tmp_mac[24], tmp_ip[20], tmp_mask[20], tmp_gateway[20];
		const char* format = "%[a-zA-Z] %s %[0-9.] %[0-9.] %[0-9.]";
		
		memset(tmp_token, 0, sizeof(tmp_token));
		memset(tmp_mac, 0, sizeof(tmp_mac));
		memset(tmp_ip, 0, sizeof(tmp_ip));
		memset(tmp_mask, 0, sizeof(tmp_mask));
		memset(tmp_gateway, 0, sizeof(tmp_gateway));

		if(sscanf(message, format, &tmp_token, tmp_mac, tmp_ip, tmp_gateway, tmp_mask) == 5 && 
			check_ip_format(tmp_ip) && check_ip_format(tmp_gateway) && check_ip_format(tmp_mask)) {
			int i;
			char hwpmac[20] = {0};
			unsigned char *p = (unsigned char *)hwaddr; 
		
			for(i = 0; i<mac_size*3-1; i++) { 
				if(i<(mac_size-1)*3) {
					sprintf(hwpmac+i,"%02x:",*p++);
				}
				else {
					sprintf(hwpmac+i,"%02x",*p++);
				}
				i = i+2;
			}

			if(strncmp(tmp_mac, hwpmac, strlen(hwpmac)) == 0) {
				sprintf(respone_msg, "%s", "ModifyNetInfoStatus:1");
				send_mac_message(sock, frommac, hwaddr, FCS1, respone_msg);
				// to do reboot
				printf("respone_msg: %s\n", respone_msg);
			}
		}
		else{
			sprintf(respone_msg, "%s", "ModifyNetInfoStatus:0");
			send_mac_message(sock, frommac, hwaddr, FCS1, respone_msg);
			printf("respone_msg: %s\n", respone_msg);
		}
	}
	else if(strcmp(message, "getdev") == 0) {	
		char FCS2[]={0x22, 0x22};
		sprintf(respone_msg,"ip:%s netmask:%s gateway:%s sn:%s masterNum:%s slaveNum:%s adressboxIp:%s deviceModel:%s consultNum:%s",
			"192.168.1.168",
			"255.255.255.0",
			"192.168.1.1",
			"A1234567823",
			"001",
			"085",
			"192.168.1.254",
			"Summer-z",
			"002");
		
		send_mac_message(sock, frommac, hwaddr, FCS2, respone_msg);
	}
	else {
		printf("unkown mac command");
	}
	
	return 0;
}

void* physicalLayerCommunictionFunction(void *param) 
{
	int i, ret, sock, on=1;
	unsigned char hwaddr[mac_size] = {0};
	unsigned char buf[bufsize] = {0};
	
	sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock < 0) {
		perror("sock");
		return -1;
	}
	printf("sock: %d\n", sock);
	
	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret < 0) {
		perror("setsockopt");
		return -1;
	}
	
	get_hwaddr("eth0",hwaddr);
	while(1){
		ret = recvfrom(sock, buf, bufsize, 0, NULL, NULL);
		if(ret < 0) {
			perror("receive error");
			return -1;
		}		
			
		if(memcmp(buf, comparision, 6) || memcmp(buf, hwaddr)){
			if(buf[protocol_pos] == 0x88){
				handle_mac_message(sock, buf, hwaddr);
			}
		}
	}
}
