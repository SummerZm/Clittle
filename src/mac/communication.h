#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include<stdlib.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <sys/types.h>  
#include <arpa/inet.h>  
#include <features.h>    /* for the glibc version number */  
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1  
#include <netpacket/packet.h>  
#include <net/ethernet.h>     /* the L2 protocols */  
#else  
#include <asm/types.h>  
#include <linux/if_packet.h>  
#include <linux/if_ether.h>   /* The L2 protocols */  
#endif  
#include <stdio.h>  
#include <netinet/in.h>  
#include <net/if.h>  
#include <sys/ioctl.h>  
#include <errno.h>  
#include<unistd.h>  
#include<ctype.h>

#define _PATH_PROCNET_DEV               "/proc/net/dev"  
#define return_data "I have gotton your message!!!!!!!!" //回复广播时附带的数据
 
#define bufsize 1024
#define src_mac_pos 6  //源mac地址在发送数据包字符串的下标是6
#define mac_size 6 //mac地址的数据长度为6bytes
#define data_size 128  //接收数据包所用数组receive_data大小
#define message_size 128 //解析数据用message数组大小
#define protocol_pos 12 //协议类型在数据包的第12位和13位
#define fcs_pos 14//fcs在receive_data[14] receive_data[15]中
#define message_length_pos 16//message大小存放在receive_data[16]中
#define message_pos 17 //message在receive_data中从第17位开始  


extern void show_data(char *str,int len);  //打印数据包的数据部分信息
extern void add_data(unsigned char* str1,char* str2);  //向数据包中增加数据部分
extern char *get_name(char *name, char *p);   //读取字符串信息，在read_netdev_proc中被调用
extern  int read_netdev_proc(void *devname, const int num, const int len);  //从 /proc/net/dev中读取网卡名称i
extern int get_hwaddr(char *name, unsigned char *hwaddr);  //获取mac地址
extern void printhex(void *hex, int len, char *tag);  //素据按十六进制分开打印
extern int compare(unsigned char* str1,unsigned char* str2);  //数据比较  比较获取的数据包的头六字节是否为自己的mac地址


#endif
