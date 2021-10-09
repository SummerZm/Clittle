/*************************************************************************
 > File Name: decry.c
 > Author: Leafxu
 > Created Time: 2020年08月07日 星期五 14时22分11秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decry(char* data, int len) {
	int i=0;
	if (data==NULL) {
		printf("decry data null\n");
		return;
	}

	if (len<0) {
		printf("decry data len %d error.\n", len);
		return;
	}

	for (i=0; i<len; i++) {
		data[i] ^= 'A';
	}
	return;
}

void getLogFileName(const char* logName, char* buffer, int len) {
	const char *start=NULL, *end=NULL;
	if (logName==NULL || buffer || len<=0) {
		printf("logName %s or buffer or len %d Invalid\n", logName, len);
		exit(-1);
	}
	start = strrchr(logName, '/');
	if (start==NULL) {
		start = logName;
	}
	
	end = strrchr(logName, '.');
	if (end==NULL) {
		strcpy(buffer, start);
		return;
	}
	
	if (end>start && len>end-start) {
		strncpy(buffer, start, end-start);
		return;
	}
	strncpy(buffer, start, len);	
}

int main(int argc, char** argv) {
	char buffer[4*1024];
	FILE *fd1, *fd2;
	if (argc<3) {
		printf("Usage: %s input[file] output[file]\n", argv[0]);
		return -1;
	}

	fd1 = fopen(argv[1], "rb");
	if (fd1==NULL) {
		perror("Open log file: ");
		return -1;
	}

	fd2 = fopen(argv[2],"w+");
	if (fd2==NULL) {
		fclose(fd1);
		perror("Create output file: ");
		return -1;
	}
	
	fseek(fd1, 0, SEEK_SET);
	while (!feof(fd1)) {
		int i=0, rBytes=0, wBytes;
		memset(buffer, 0, sizeof(buffer));
		rBytes = fread(buffer, 1, sizeof(buffer), fd1);
		if (rBytes<sizeof(buffer) && ferror(fd1)) {
			perror("Read log file error: ");
			break;
		}
		decry(buffer, rBytes);
		wBytes= fwrite(buffer, 1, rBytes, fd2);
		if (wBytes<rBytes) {
			printf("Need write %d but actually %d only.\n", rBytes, wBytes);
		}
	}
	fflush(fd2);
	fclose(fd1);
	fclose(fd2);
	return 0;
}

