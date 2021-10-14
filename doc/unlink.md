头文件：#include <unistd.h>

定义函数：int unlink(const char * pathname);

函数说明：unlink()会删除参数pathname 指定的文件. 如果该文件名为最后连接点, 但有其他进程打开了此文件, 则在所有关于此文件的文件描述词皆关闭后才会删除. 如果参数pathname 为一符号连接, 则此连接会被删除。

返回值：成功则返回0, 失败返回-1, 错误原因存于errno

错误代码：
1、EROFS 文件存在于只读文件系统内。
2、EFAULT 参数pathname 指针超出可存取内存空间。
3、ENAMETOOLONG 参数pathname 太长。
4、ENOMEM 核心内存不足。
5、ELOOP 参数pathname 有过多符号连接问题。
6、EIO I/O 存取错误。
