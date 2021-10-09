
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define HOST_NAME_MAX	256

#define NTRIES		5

#define SERV1		"192.168.1.137"
#define SERV2		"192.168.1.138"
#define SERV3		"192.168.1.139"

#define SERVPORT	9856	/* Public port for requests from clients */
#define PRIVPORT	9857	/* Private port for bounce requests only */

#define REQMAGIC	0x76849268
#define REPLMAGIC	0x01967293

struct request {
	uint32_t magic;
};

struct reply {
	uint32_t magic;
	struct in_addr pubaddr;
	uint16_t pubport;
};

int server;
int verbose = 0;

/* Internet addresses of server 3 */
struct sockaddr_in sin3;

void perrordie(const char *msg) {
	perror(msg);
	exit(1);
}

void getaddr(const char *hostname, struct in_addr *addr)
{
	struct hostent *h;

	h = gethostbyname(hostname);
	if (h == 0)
		perrordie(hostname);
	if (h->h_addrtype != AF_INET) {
		fprintf(stderr, "%s: unexpected address type %d\n",
				hostname, h->h_addrtype);
	}
	*addr = *(struct in_addr*)(h->h_addr);
}

int mksock(int type)
{
	int sock = socket(AF_INET, type, 0);
	if (sock < 0)
		perrordie("socket");
	return sock;
}

void setblock(int sock)
{
	int fl = fcntl(sock, F_GETFL);
	if (fl < 0)
		perrordie("fcntl");
	if (fcntl(sock, F_SETFL, fl & ~O_NONBLOCK) < 0)
		perrordie("fcntl");
}

void setnonblock(int sock)
{
	int fl = fcntl(sock, F_GETFL);
	if (fl < 0)
		perrordie("fcntl");
	if (fcntl(sock, F_SETFL, fl | O_NONBLOCK) < 0)
		perrordie("fcntl");
}

void setreuse(int sock)
{
	const int one = 1;

#ifdef SO_REUSEADDR
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
		perrordie("setsockopt(SO_REUSEADDR)");
#endif
#ifdef SO_REUSEPORT
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)) < 0)
		perrordie("setsockopt(SO_REUSEPORT)");
#endif
}

void settimeouts(int sock, int secs)
{
	struct timeval tv;

	tv.tv_sec = secs;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
		perrordie("setsockopt(SO_SNDTIMEO)");
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		perrordie("setsockopt(SO_RCVTIMEO)");
}


/* Serve a TCP request if we're a regular server (1-2) */
void servetcp(int tcpsock)
{
	struct sockaddr_in sin;
	socklen_t sinlen;
	int clisock;
	struct request rq;
	struct reply rp;
	int rc;

	sinlen = sizeof(sin);
	clisock = accept(tcpsock, (struct sockaddr*)&sin, &sinlen);
	if (clisock < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			perrordie("accept");
		return;
	}

	/* Fork off a child to handle this connection */
	rc = fork();
	if (rc < 0)
		perrordie("fork");
	if (rc == 0) {
		/* In the parent - close clisock and go back for more */
		close(clisock);
		return;
	}

	/* In the child */

	/* Put the socket in blocking mode */
	setblock(clisock);

	/* Make sure we don't linger a long time if the other end disappears */
	settimeouts(clisock, 20);

	/* Read the request */
	rc = read(clisock, &rq, sizeof(rq));
	assert(rc <= sizeof(rq));
	if (rc < sizeof(rq)) {
		if (verbose)
			fprintf(stderr, "Runt TCP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		exit(0);	/* drop */
	}
	if (rq.magic != htonl(REQMAGIC)) {
		if (verbose)
			fprintf(stderr,
				"Invalid TCP request %08x from %s:%d\n",
					rq.magic,
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		exit(0);	/* drop */
	}

	if (verbose)
		fprintf(stderr, "TCP request from %s:%d\n",
				inet_ntoa(sin.sin_addr),
				ntohs(sin.sin_port));

	/* If we're server 2,
	   then first send a "bounce request" to server 3 */
	if (server == 2) {
		int s3sock;

		s3sock = socket(AF_INET, SOCK_STREAM, 0);
		if (s3sock < 0)
			perrordie("socket s3sock");

		settimeouts(s3sock, 20);

		if (connect(s3sock, (const struct sockaddr*)&sin3,
				sizeof(sin3)) < 0)
			perrordie("connect s3sock");

		/* Send the bounce request */
		rp.magic = htonl(REQMAGIC);
		rp.pubaddr = sin.sin_addr;
		rp.pubport = sin.sin_port;
		if (write(s3sock, &rp, sizeof(rp)) < 0)
			perrordie("write s3sock");

		/* Wait for server 3's reply,
		   signaling that we should go ahead and reply to the client.
		   We don't expect any data, just a clean socket close. */
		if (read(s3sock, &rp, 1) < 0)
			perrordie("read s3sock");

		close(s3sock);
	}

	/* Build the reply and send it back to the client */
	rp.magic = htonl(REPLMAGIC);
	rp.pubaddr = sin.sin_addr;
	rp.pubport = sin.sin_port;
	if (write(clisock, &rp, sizeof(rp)) < 0)
		perrordie("sendto");

	close(clisock);
	exit(0);
}

/* Serve a UDP request if we're a regular server (1-2) */
void serveudp(int udpsock)
{
	struct sockaddr_in sin;
	socklen_t sinlen;
	struct request rq;
	struct reply rp;
	int rc;

	sinlen = sizeof(sin);
	rc = recvfrom(udpsock, &rq, sizeof(rq), 0,
			(struct sockaddr*)&sin, &sinlen);
	if (rc < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			perror("recvmsg");
		return;
	}
	if (rc < sizeof(rq)) {
		if (verbose)
			fprintf(stderr, "Runt UDP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		return;	/* drop */
	}
	if (rq.magic != htonl(REQMAGIC)) {
		if (verbose)
			fprintf(stderr, "Invalid UDP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		return;	/* drop */
	}

	if (verbose)
		fprintf(stderr, "UDP request from %s:%d\n",
				inet_ntoa(sin.sin_addr),
				ntohs(sin.sin_port));

	/* Build the reply and send it to the client */
	rp.magic = htonl(REPLMAGIC);
	rp.pubaddr = sin.sin_addr;
	rp.pubport = sin.sin_port;
	if (sendto(udpsock, &rp, sizeof(rp), 0,
			(struct sockaddr*)&sin, sizeof(sin)) < 0)
		perror("sendto");

	/* If we're server 2,
	   then also send it as a "bounce request" to server 3 */
	if (server == 2) {
		if (verbose)
			fprintf(stderr, "Forwarding bounce request to %s:%d\n",
				inet_ntoa(sin3.sin_addr), ntohs(sin3.sin_port));
		rp.magic = htonl(REQMAGIC);
		if (sendto(udpsock, &rp, sizeof(rp), 0,
				(struct sockaddr*)&sin3, sizeof(sin3)) < 0)
			perror("sendto");
	}
}

void servemain()
{
	int tcpsock, udpsock;
	struct sockaddr_in sin;
	int rc;

	/* Lookup our special "bounce" server */
	sin3.sin_family = AF_INET;
	getaddr(SERV3, &sin3.sin_addr);
	sin3.sin_port = htons(PRIVPORT);
	if (verbose)
		fprintf(stderr, "bounce server %s at %s\n",
				SERV3, inet_ntoa(sin3.sin_addr));

	/* Make the sockets we need */
	tcpsock = mksock(SOCK_STREAM);
	udpsock = mksock(SOCK_DGRAM);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVPORT);
	if (bind(tcpsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind tcpsock");
	if (bind(udpsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind udpsock");

	/* Make the sockets nonblocking */
	setnonblock(tcpsock);
	setnonblock(udpsock);

	/* Listen on the TCP socket. */
	if (listen(tcpsock, 0) < 0)
		perrordie("listen");

	/* Become a daemon */
	rc = fork();
	if (rc < 0)
		perrordie("fork");
	if (rc != 0)
		exit(0);

	/* Loop forever receiving messages and sending pings */
	while (1) {
		int maxsock = 0;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(tcpsock, &fds); maxsock = max(maxsock, tcpsock);
		FD_SET(udpsock, &fds); maxsock = max(maxsock, udpsock);
		rc = select(maxsock+1, &fds, 0, 0, 0);
		if (rc < 0)
			perrordie("select");

		servetcp(tcpsock);
		serveudp(udpsock);

		/* Collect any zombie children */
		while (waitpid(-1, 0, WNOHANG) >= 0);
	}
}

/* Serve a TCP request if we're a bounce server (3) */
void bouncetcp(int tprivsock)
{
	struct sockaddr_in sin;
	socklen_t sinlen;
	int clisock, dstsock;
	struct reply rp;
	fd_set fds, tfds;
	struct timeval tv;
	int rc;

	sinlen = sizeof(sin);
	clisock = accept(tprivsock, (struct sockaddr*)&sin, &sinlen);
	if (clisock < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			perrordie("accept");
		return;
	}

	/* Fork off a child to handle this connection */
	rc = fork();
	if (rc < 0)
		perrordie("fork");
	if (rc == 0) {
		/* In the parent - close clisock and go back for more */
		close(clisock);
		return;
	}

	/* In the child */

	/* Put the socket in blocking mode */
	setblock(clisock);

	/* Make sure we don't linger a long time if the other end disappears */
	settimeouts(clisock, 20);

	/* Read the request */
	rc = read(clisock, &rp, sizeof(rp));
	if (rc < sizeof(rp)) {
		if (verbose)
			fprintf(stderr, "Runt TCP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		exit(0);	/* drop */
	}
	if (rp.magic != htonl(REQMAGIC)) {
		if (verbose)
			fprintf(stderr, "Invalid TCP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		exit(0);	/* drop */
	}

	if (verbose) {
		fprintf(stderr, "TCP bounce request from %s:%d ",
				inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		fprintf(stderr, "to %s:%d\n",
				inet_ntoa(rp.pubaddr), ntohs(rp.pubport));
	}

	/* Turn the request into a reply */
	rp.magic = htonl(REPLMAGIC);

	/* Create a new socket to perform the requested bounce-callback */
	dstsock = socket(AF_INET, SOCK_STREAM, 0);
	if (dstsock < 0)
		perrordie("socket");

	/* Make sure it comes from our regular server port */
	setreuse(dstsock);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVPORT);
	if (bind(dstsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind dstsock");

	/* Start a nonclocking connection attempt to the client */
	setnonblock(dstsock);
	sin.sin_addr = rp.pubaddr;
	sin.sin_port = rp.pubport;
	if (connect(dstsock, (const struct sockaddr*)&sin, sizeof(sin)) < 0) {
		if (errno != EINPROGRESS)
			perrordie("connect dstsock");
	}

	FD_ZERO(&fds);
	FD_SET(dstsock, &fds);

	/* Wait a few seconds to make sure the first couple SYNs
	   have a chance to reach the firewall (and hopefully be dropped)
	   before signaling the client to initiate a simultaneous
	   outgoing connection to us. */
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	tfds = fds;
	if (select(dstsock+1, 0, &tfds, 0, &tv) < 0)
		perrordie("select dstsock");
	if (FD_ISSET(dstsock, &tfds)) {
		/* The firewall didn't prevent our incoming connection. */
		if (verbose)
			fprintf(stderr,
				"Callback connection finished prematurely\n");
	} else {
		/* The right thing happened -
		   the firewall appears to be dropping our SYNs to the client.
		   So signal server 2 to send its reply to the client,
		   which will cause the client to initiate its open to us. */
		close(clisock);
		clisock = -1;

		if (verbose)
			fprintf(stderr,
				"Signaling client to initiate connection\n");

		/* Wait until dstsock connects or fails. */
		tfds = fds;
		if (select(dstsock+1, 0, &tfds, 0, 0) < 0)
			perrordie("select dstsock");
		assert(FD_ISSET(dstsock, &tfds));

		if (verbose)
			fprintf(stderr, "Callback connection established\n");
	}

	/* Now place the callback socket back in blocking mode. */
	setblock(dstsock);

	/* Push the reply to the client */
	rc = write(dstsock, &rp, sizeof(rp));
	if (rc < 0)
		perrordie("write dstsock");

	if (verbose)
		fprintf(stderr, "Callback connection to %s:%d complete\n",
			inet_ntoa(rp.pubaddr), ntohs(rp.pubport));

	/* If the connection completed prematurely,
	   wait five more seconds before uncorking server 2's reply
	   to ensure that the client has time to notice
	   that the connection got through too early. */
	if (clisock >= 0) {
		sleep(5);
		close(clisock);
	}

	close(dstsock);
	exit(0);
}

/* Serve a UDP request if we're a bounce server (3) */
void bounceudp(int udpsock, int uprivsock)
{
	struct sockaddr_in sin;
	socklen_t sinlen;
	struct reply rp;
	int rc;

	sinlen = sizeof(sin);
	rc = recvfrom(uprivsock, &rp, sizeof(rp), 0,
			(struct sockaddr*)&sin, &sinlen);
	if (rc < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			perrordie("recvmsg");
		return;
	}
	if (rc < sizeof(rp)) {
		if (verbose)
			fprintf(stderr, "Runt UDP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		return;	/* drop */
	}
	if (rp.magic != htonl(REQMAGIC)) {
		if (verbose)
			fprintf(stderr, "Invalid UDP request from %s:%d\n",
					inet_ntoa(sin.sin_addr),
					ntohs(sin.sin_port));
		return;	/* drop */
	}

	if (verbose) {
		fprintf(stderr, "UDP bounce request from %s:%d ",
				inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		fprintf(stderr, "to %s:%d\n",
				inet_ntoa(rp.pubaddr), ntohs(rp.pubport));
	}

	/* Turn it into a reply */
	rp.magic = htonl(REPLMAGIC);

	/* Echo the reply to the client */
	sin.sin_family = AF_INET;
	sin.sin_addr = rp.pubaddr;
	sin.sin_port = rp.pubport;
	if (sendto(udpsock, &rp, sizeof(rp), 0,
			(struct sockaddr*)&sin, sizeof(sin)) < 0)
		perror("sendto");
}

void bouncemain()
{
	int udpsock, tprivsock, uprivsock;
	struct sockaddr_in sin;
	int rc;

	/* Only bind a UDP socket to SERVSOCK */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	udpsock = mksock(SOCK_DGRAM);
	sin.sin_port = htons(SERVPORT);
	if (bind(udpsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind udpsock");

	/* Bind both TCP and UDP sockets to PRIVSOCK */
	tprivsock = mksock(SOCK_STREAM);
	uprivsock = mksock(SOCK_DGRAM);
	sin.sin_port = htons(PRIVPORT);
	if (bind(tprivsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind tprivsock");
	if (bind(uprivsock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		perrordie("bind uprivsock");

	/* Make the sockets nonblocking */
	setnonblock(udpsock);
	setnonblock(tprivsock);
	setnonblock(uprivsock);

	/* Listen on the TCP private socket. */
	if (listen(tprivsock, 0) < 0)
		perrordie("listen");

	/* Become a daemon */
	rc = fork();
	if (rc < 0)
		perrordie("fork");
	if (rc != 0)
		exit(0);

	/* Loop forever receiving messages and sending pings */
	while (1) {
		int maxsock = 0;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(tprivsock, &fds); maxsock = max(maxsock, tprivsock);
		FD_SET(uprivsock, &fds); maxsock = max(maxsock, uprivsock);
		rc = select(maxsock+1, &fds, 0, 0, 0);
		if (rc < 0)
			perrordie("select");

		bouncetcp(tprivsock);
		bounceudp(udpsock, uprivsock);

		/* Collect any zombie children */
		while (waitpid(-1, 0, WNOHANG) >= 0);
	}
}

int main(int argc, char **argv)
{
	if (argc >= 2 && strcmp(argv[1], "-v") == 0) {
		verbose = 1;
		argc--, argv++;
	}
	if (argc != 2) {
		usage:
		fprintf(stderr, "Usage: natserver {1,2,3}\n");
		exit(1);
	}
	server = atoi(argv[1]);
	if (server < 1 || server > 3)
		goto usage;

	/* Now run the appropriate server code */
	if (server < 3)
		servemain();
	else
		bouncemain();

	return 0;
}

