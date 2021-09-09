#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ZM_PROXY_SERVER_IP		"127.0.0.1"
#define ZM_PROXY_SERVER_PORT	6009
#define ZM_PROXY_MSG_LEN		2048

#define ZM_PROXY_CMD_PROTOCOL	"{'version':'%[^']','app':'%[^']','request':'%[^']'}"

#define ZM_PROXY_PROTOCOL_CMD_LEN			256
#define ZM_PROXY_PROTOCOL_VER_LEN			48
#define ZM_PROXY_PROTOCOL_APP_NAME_LEN		128

int send_proxy_data(int proxySock, const struct sockaddr_in *client, int clientAddrLen, const char* msg, int len) {
	if (client==NULL || msg==NULL || len<=0 || clientAddrLen<0) {
		printf("client or msg[%s] or len[%d] or clientAddrLen[%d] is error. Do nothing. Exist!\n", msg, len, clientAddrLen);
		return 0;
	}
	
	if (proxySock<0) {
		printf("proxySock [%d] error. Do nothing. Exist!\n", proxySock);
		return 0;
	}

	{	
		int sendBytes = sendto(proxySock, msg, len, 0, (struct sockaddr*)client, clientAddrLen);
		if (sendBytes != len) {
			printf("Send to [%s] error. Actually send [%d] want send [%d]\n", inet_ntoa(client->sin_addr), sendBytes, len);
			perror("Sendto ");
		}
		return sendBytes;
	}
} 

int main(int argc, char** argv) {
	const char* proxyServerIp = ZM_PROXY_SERVER_IP;
	// cmdline input
	{
		if (argc==2) 
		{
			if (strncmp("-m", argv[1], 2)==0) {
				proxyServerIp = "0.0.0.0";
			}

			if (strncmp("-help", argv[1], 5)==0) {
				printf("ZM_proxy default bind ip is %s\n", proxyServerIp);
				printf("-m  ZM_proxy bind ip is 0.0.0.0\n");
				return 0;
			}
	
			if (strncmp("-v", argv[1], 2)==0) {
				printf("ZM_proxy: %s %s\n", __DATE__, __TIME__);
				return 0;
			}
		}
	}

	// Init udp sokect 
	struct sockaddr_in serverAddr;
	int proxySock = socket(AF_INET, SOCK_DGRAM, 0);
	if (proxySock == -1) {
		perror("Socket create");
		return 0;
	}
	// Init server addr.
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(proxyServerIp);
	serverAddr.sin_port = htons(ZM_PROXY_SERVER_PORT);

	// Bind 
	if (-1 == bind(proxySock, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
		perror("Socket bind");
		goto RES_FREE_EIXT;
	}

	// Set no-block
	{
		int iSockFlags = fcntl(proxySock, F_GETFL, 0);
		iSockFlags |= O_NONBLOCK;
		if (-1 == fcntl(proxySock, F_SETFL, iSockFlags)) {
			perror("fcntl set no-block");
			goto RES_FREE_EIXT;
		}
	}

	// Recive request
	while(1) {
		struct sockaddr_in clientAddr;
		int socketLen = sizeof(clientAddr);
		int recvBytes = 0;
		char recvData[ZM_PROXY_MSG_LEN];
		memset(recvData, 0, sizeof(recvData));
		memset(&clientAddr, 0, sizeof(clientAddr));
	
		recvBytes = recvfrom(proxySock, recvData, sizeof(recvData), 0, (struct sockaddr*)&clientAddr,&socketLen);
		if (ZM_PROXY_MSG_LEN < recvBytes) {
			printf("Receive [%s] msglen [%d] too large. drop.\n", inet_ntoa(clientAddr.sin_addr), recvBytes);
		}
		else if (recvBytes < 0) {
			//printf("Receive [%s] error\n", inet_ntoa(clientAddr.sin_addr));
			//perror("recvfrom error ");
		}
		else {
			printf("Receive data [%s]\n", recvData);
			{
				int ret = 0;
				char cmd[ZM_PROXY_PROTOCOL_CMD_LEN];
				char app[ZM_PROXY_PROTOCOL_APP_NAME_LEN];
				char version[ZM_PROXY_PROTOCOL_VER_LEN];
				memset(cmd, 0, sizeof(ZM_PROXY_PROTOCOL_CMD_LEN));
				memset(app, 0, sizeof(ZM_PROXY_PROTOCOL_APP_NAME_LEN));
				memset(version, 0, sizeof(ZM_PROXY_PROTOCOL_VER_LEN));
				ret = sscanf(recvData, ZM_PROXY_CMD_PROTOCOL, version, app, cmd);
				if (ret==3) 
				{
					printf("App %s \n", app);
					printf("Version %s \n", version);
					printf("Execute cmd %s \n", cmd);
					system(cmd);
				}
				else {
					printf("Protocol %s analize failed! ret:[%d]\n", ZM_PROXY_CMD_PROTOCOL, ret);
					perror("sscanf ");
				}
			}
			send_proxy_data(proxySock,  &clientAddr, sizeof(clientAddr), "OK1", 3);
		}
		//sleep(1);
	}

RES_FREE_EIXT:
	if (proxySock>=0) { close(proxySock); }
	return 0;
}
