#include"communication.h"

unsigned char comparision[]={//用于验证接收到的数据是否为广播数据，在compare中被调用
	0xff,0xff,0xff,0xff,0xff,0xff
};

int main(int argc, char **argv)  
{  
	int i,ret,sock,ret1; 
	int on=1,length; 
	unsigned char hwaddr[mac_size];  
	char devname[3][7],message[message_size]; 
	char frommac[mac_size];
	char FCS[]={0x22,0x22};
	char temp_ip[20] = {0};
    char temp_mask[20] = {0};
    char temp_gateway[20] = {0};
    char temp_msg[100] = {0};

	unsigned char buf[bufsize];   


	sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  
	if(sock < 0){  
		perror("sock");  
		return -1;  
	}  

	ret1=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(ret1<0){
		perror("setsockopt");
		exit(1);
	}

	struct sockaddr_ll sll;  
	
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "eth0");
	ioctl(sock, SIOCGIFINDEX, &ifr);
	memset(&sll, 0, sizeof(sll)); 
	sll.sll_pkttype=PACKET_HOST; 
	sll.sll_ifindex = ifr.ifr_ifindex;


	read_netdev_proc(devname, 3, 7);  
	for(i = 0; i < 3 && get_hwaddr(devname[i], hwaddr) != 0; i++){     
		//empty  
	}  
	while(1){
		ret = recvfrom(sock, buf,bufsize, 0, NULL, NULL); 
		if(ret<0){
			perror("receive error");
			exit(1);
		}

		if(!compare(buf,comparision)||!compare(buf,hwaddr)){

			if(buf[protocol_pos]==0x88){ 
				bzero(message,message_size);
				//printf("data length is: %d\n",buf[message_length_pos]);
				memcpy(message,buf+message_pos,buf[message_length_pos]);
				printf("receive data is :%s\n",message);
				char* token=strtok(message," ");
				if(token != 0 && strlen(token) > 0)
				{
					if(strcmp(token,"ModifyNetInfo")==0)
					{
						token=strtok(NULL," ");
						if(token == 0 || strlen(token) == 0) 
						{
							printf("get mac error\n");
							continue;
						}
						char tmpmac[20] = {0};
						unsigned char *p = (unsigned char *)hwaddr; 
						int i;
						for(i = 0; i<mac_size*3-1; i++){ 
							if(i<(mac_size-1)*3)
							{
								sprintf(tmpmac+i,"%02x:",*p++);
							}
							else
							{
								sprintf(tmpmac+i,"%02x",*p++);
							}
							i = i+2;
						}
						if(strncmp(token,tmpmac,strlen(token)) == 0)
						{
							printf("get setip command\n");
							token=strtok(NULL," ");
							if(token == 0 || strlen(token) == 0) {
								printf("get config ip error\n");
								continue;
							}
							int ret = SetIfAddr("eth0",token);
							printf("set ip ret:%d\n",ret);
						}
					}
					else if(strcmp(token,"getdev")==0) {}
					else {
						printf("unknow command.\n");
						continue;
					}  
				}
			}else
			{
				continue;
			}  


			//printf("the following is my board getting:\n");
			//printhex(buf,ret," ");

			bzero(frommac,mac_size);
			memcpy(frommac,buf+mac_size,mac_size);

			printf("from mac:");
			printhex(frommac,mac_size," ");          
			printf("local mac:");   
			printhex(hwaddr, mac_size, " "); 

			memcpy(buf,frommac,mac_size);  
			memcpy(buf+mac_size, hwaddr, mac_size); 
			memcpy(buf+fcs_pos,FCS,strlen(FCS));
			//memcpy(buf+message_pos,temp_ip,strlen(temp_ip));

			get_ip(temp_ip);
            		GetLocalNetMask(temp_mask);
            		GetGateWay(temp_gateway);
            		sprintf(temp_msg,"ip:%s netmask:%s gateway:%s",temp_ip,temp_mask,temp_gateway);
			add_data(buf,temp_msg);
			length=message_pos+strlen(temp_msg);          

			ret1=sendto(sock,buf,length,0,(const struct sockaddr*)&sll,sizeof(sll));
			if(ret1<0){
				printf("发送的时候出错！");
				perror("send error");
				exit(1);
			}
			printf("my return boarding is:\n");
			printhex(buf,ret1," ");
			printf("my return data is:%s\n",buf+message_pos);  

		}
	}  

	return 0;  
}  
