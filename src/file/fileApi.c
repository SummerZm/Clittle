/*************************************************************************
 > File Name: fileSize.c
 > Author: Leafxu
 > Created Time: 2020年07月08日 星期三 12时11分58秒
 ************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * @file: Full path of file.
 * @Return: Totall bytes. 
 */
int fileSize(const char* file) {
	struct stat info;
	if (file==NULL) return -1;
	if (stat(file, &info)<0) return -1;
	return info.st_size;
}

int isDir(const char* file) {
	struct stat info;
	if (file==NULL) return -1;
	if (stat(file, &info)<0) return -1;
	return S_ISDIR(info.st_mode);
}

/* #include "unistd.h"
 * @ R_OK
 * @ W_OK
 * @ F_OK [if file exist]
 * @ X_OK
 */
int isExecutable(const char* file) {
	if (file==NULL) return -1;
	return access(file, X_OK);
}

/* 
 * @packageName: The name of package.
 * @path: The location of package.
 * @tarOject: The full path of target file or directory.
 * @return: -1/0
 */
int tarFile(const char* packageName, const char* path, const char* tarOject) {	
    char cmd[1024];
	char currPath[1024];
    if (packageName==NULL || path==NULL || tarOject==NULL) {
        return -1;
    }
    memset(currPath, 0,sizeof(currPath));
    memset(cmd, 0 , sizeof(cmd));
    getcwd(currPath, sizeof(currPath));
	sprintf(cmd, "tar -jcvf %s %s", packageName, tarOject);
	printf("currPath:%s execuPath:%s cmd: %s\n", currPath, path, cmd);
	chdir(path);
	system(cmd);
	chdir(currPath);
	return 0;
}

/*
 * @Rerge file2 to file1.
 * @Return: 0/1 [Fial/OK]
 */
int mergefile(const char* file1, const char* file2) {
    int ret = 0;
    char buffer[1024*4];
    FILE *fp1=NULL, *fp2=NULL;
    if (file1==NULL || file2==NULL) {
        printf("file1:%s or file2:%s is null\n", file1, file2);
        return ret;
    }

    fp1 = fopen(file1, "a+");
    if (fp1==NULL) {
        printf("open file %s failed.\n", file1);
        return ret;
    }

    fp2 = fopen(file2, "r");
    if (fp2==NULL) {
        printf("open file %s failed.\n", file1);
        fclose(fp1);
        return ret;
    }
    
    memset(buffer, 0, sizeof(buffer));
    while (1==(fread(buffer, sizeof(buffer), 1, fp2))) {
        if (1==fwrite(buffer, sizeof(buffer), 1, fp1)) {
            memset(buffer, 0, sizeof(buffer));
        }
        else {
            printf("Merge file %s to %s fail in fwrite().\n", file2, file1);
            fclose(fp1);
            fclose(fp2);
            return 0;
        }
    }
    if (feof(fp2)) {
        ret = fwrite(buffer, strlen(buffer), 1, fp1);
        printf("Merge file %s to %s finish. ret:%d\n", file2, file1, ret);
    }
    fclose(fp1);
    fclose(fp2);
    return ret;
}

