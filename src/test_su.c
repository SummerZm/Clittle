#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <linux/capability.h>
/* There doesn't seem to be a universally-available
   userpace header for these. */
extern int capset(cap_user_header_t header, cap_user_data_t data);
extern int capget(cap_user_header_t header, cap_user_data_t data);
#define LINUX_CAPABILITY_VERSION_1  0x19980330
#define LINUX_CAPABILITY_VERSION_2  0x20071026
#define LINUX_CAPABILITY_VERSION_3  0x20080522


extern char**environ;

int printf_env() {
	char**var;
	printf("PPid: %d", getppid());
	if (environ!=NULL) {
		for (var =environ;*var !=NULL;++var) {
			printf ("%s \n",*var);
		}
	}
	return 0;
}

int test_net()
{
	struct sockaddr_in sinAddress;
	int portNumber = 443;
	int opt = 1;
	int iRc = 0;
	int iListenSocket;
	iListenSocket=socket(AF_INET,SOCK_STREAM,0);
	memset(&sinAddress,0,sizeof(struct sockaddr_in));
	sinAddress.sin_family=AF_INET;
	sinAddress.sin_addr.s_addr=htonl(INADDR_ANY);
	sinAddress.sin_port=htons(portNumber); // http port
	setsockopt(iListenSocket, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
	iRc=bind(iListenSocket,(struct sockaddr*)&sinAddress, sizeof(struct sockaddr_in));
	if(iRc<0){
		perror("bind :");
		printf("Failed to bind port[%d]",portNumber);
		return -1;
	}
	close(iListenSocket);
	printf("Bind socket test ok\n");
	return 0;
}

int set_process_env()
{
	uid_t child_uid = 1001; // user system
	uid_t child_gid = 1001; // user system

	// creates a session
	{
		setsid();
		prctl(PR_SET_NAME, "test_su_child");
	}

	// set capabilities
	{
		int capsize = 1; /* for header version 1 */
		cap_user_header_t hdr = malloc(sizeof(*hdr));
		cap_user_data_t data = NULL;

		memset(hdr, 0, sizeof(*hdr));
		capget(hdr, NULL);
		if (hdr->version != LINUX_CAPABILITY_VERSION_1) {
			if (hdr->version != LINUX_CAPABILITY_VERSION_2)
				hdr->version = LINUX_CAPABILITY_VERSION_3;
			capsize = 2;
		}
	
		data = malloc(sizeof(*data) * capsize);
		memset(data, 0, sizeof(*data) * capsize);
		data->effective = data->permitted = data->inheritable =
		(1 << CAP_NET_BIND_SERVICE) |
		(1 << CAP_NET_ADMIN) | (1 << CAP_NET_RAW) |
		(1 << CAP_SETUID) | (1 << CAP_SETGID);
		if (capset(hdr, data) == -1 || prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) == -1) {
			printf("%s: capset or PR_SET_KEEPCAPS error: %s\n", __func__, strerror(errno));
		}

		// change user to system
	if (setgid(child_gid) == -1) {
		printf("%s: setgid(%d) error: %s\n", __func__, child_gid, strerror(errno));
	}
	if (setuid(child_uid) == -1) {
		printf("%s: setuid(%d) error: %s\n", __func__, child_uid, strerror(errno));
	}

	/* lose the setuid and setgid capbilities */
	data->effective = data->permitted =
		(1 << CAP_NET_BIND_SERVICE) |
		(1 << CAP_NET_ADMIN) | (1 << CAP_NET_RAW);
	data->inheritable = 0;
	if (capset(hdr, data) == -1) {
		printf("%s: capset error: %s\n", __func__, strerror(errno));
	}	

	printf("%s: child process pid=%d,uid=%d,gid=%d\n",
			__func__, getpid(), getuid(), getgid());
	}
}

int child_process(int pipefd[2])
{
	char buffer[1024];
	uid_t child_uid = 1001; // user system
	uid_t child_gid = 1001; // user system
	printf("Root env:\n");
	printf_env();
	printf("\n");

	// creates a session
	setsid();

	prctl(PR_SET_NAME, "test_su_child");

	// set capabilities
	int capsize = 1; /* for header version 1 */
	cap_user_header_t hdr = malloc(sizeof(*hdr));
	cap_user_data_t data = NULL;

	memset(hdr, 0, sizeof(*hdr));
	capget(hdr, NULL);
	if (hdr->version != LINUX_CAPABILITY_VERSION_1) {
		if (hdr->version != LINUX_CAPABILITY_VERSION_2)
			hdr->version = LINUX_CAPABILITY_VERSION_3;
		capsize = 2;
	}
	data = malloc(sizeof(*data) * capsize);
	memset(data, 0, sizeof(*data) * capsize);
	data->effective = data->permitted = data->inheritable =
		(1 << CAP_NET_BIND_SERVICE) |
		(1 << CAP_NET_ADMIN) | (1 << CAP_NET_RAW) |
		(1 << CAP_SETUID) | (1 << CAP_SETGID);
	if (capset(hdr, data) == -1 || prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) == -1) {
		printf("%s: capset or PR_SET_KEEPCAPS error: %s\n", __func__, strerror(errno));
	}

	// change user to system
	if (setgid(child_gid) == -1) {
		printf("%s: setgid(%d) error: %s\n", __func__, child_gid, strerror(errno));
	}
	if (setuid(child_uid) == -1) {
		printf("%s: setuid(%d) error: %s\n", __func__, child_uid, strerror(errno));
	}

	/* lose the setuid and setgid capbilities */
	data->effective = data->permitted =
		(1 << CAP_NET_BIND_SERVICE) |
		(1 << CAP_NET_ADMIN) | (1 << CAP_NET_RAW);
	data->inheritable = 0;
	if (capset(hdr, data) == -1) {
		printf("%s: capset error: %s\n", __func__, strerror(errno));
	}	

	printf("%s: child process pid=%d,uid=%d,gid=%d\n",
			__func__, getpid(), getuid(), getgid());
	printf_env();

	close(pipefd[1]);          /* Close unused write end */

	if (setuid(0) == -1) {
		printf("%s: setuid(0) error: %s\n", __func__, strerror(errno));
	}

	// A. 在控制终端执行 "su -c my_exe - normaluser", 终端不一定能完全的能力/权限
	// B. 用root 用户fork 出子进程，在子进程里调用system(my_exe), 
	//  system 
	system("my_exe &");
	execvp("my_exe", NULL);
	//test_net();
	/*
	for (;;) {
		// Process event with parent process
		if (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
			printf("%s: child process pid=%d,uid=%d,gid=%d\n",
                    __func__, getpid(), getuid(), getgid());
			printf("%s: read buffer:%s\n", __func__, buffer);
			if (strstr(buffer, ",exit=1")) {
				printf("%s: main process is exited.\n", __func__);
				break;
		}
	}*/
	close(pipefd[0]);
	_exit(EXIT_SUCCESS);

}

int main(int argc, char *argv[])
{
	int pipefd[2];
	pid_t cpid;
	char buf;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {    /* Child reads from pipe */
		child_process(pipefd);
	} else {            /* Parent writes to pipe */
		int i;
		close(pipefd[0]);          /* Close unused read end */
		// Process event with child process
		for (i = 0; i <= 10; i++) {
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "[main process: i=%d,pid=%d,uid=%d,gid=%d,exit=%d]",
					i, getpid(), getuid(), getgid(), i == 10);
			write(pipefd[1], buffer, strlen(buffer));
			sleep(5);
		}
		close(pipefd[1]);          /* Reader will see EOF */
		//wait(NULL);                /* Wait for child */
		exit(EXIT_SUCCESS);
	}
}

