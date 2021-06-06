#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<memory.h>

void out_host(struct hostent *h){
  //获取主机名称
  char *host_name = h->h_name;
  printf("host_name:%s\n",host_name);

  //获取网络类型
  int host_addrtype = h->h_addrtype;
  if(host_addrtype == AF_INET){
    printf("addrtype:IPV4\n");
  }else{
    printf("addrtype:IPV6\n");
  }
  //获取主机网络地址的大小
  int host_length = h->h_length;
  printf("host_length:%d\n",host_length);

  //获取主机的ip,主机的ip对应的是网络地址数组中的0,记住要将网络字节序转换成主机字节序号
  char ip[16];
  memset(ip,0,sizeof(ip));
  inet_ntop(h->h_addrtype,h->h_addr_list[0],ip,sizeof(ip));
  printf("host ip:%s\n",ip);

  //遍历去获取别名
  int aliase_count = 0;
  while( h->h_aliases[aliase_count] != NULL){
    printf("host_aliases[%d]:%s\n",aliase_count,h->h_aliases[aliase_count]);
    aliase_count++;
 }
  int host_addr_list_count = 0;
  while(h->h_addr_list[host_addr_list_count] != NULL){
    memset(ip,0,sizeof(ip));
    inet_ntop(h->h_addrtype,h->h_addr_list[host_addr_list_count],ip,sizeof(ip));
    printf("host_addr_list[%d]:%s\n",host_addr_list_count,ip);
    host_addr_list_count++;
  }
}

int main(int argc,char *argv[]){
  if(argc < 2){
    printf("usage: %s host\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  struct hostent *h;

  //因为gethost返回的是一个结构体数组
  while((h = gethostent()) != NULL){
    //  out_host(h);
    if(strcmp(argv[1],h->h_name) == 0){
      out_host(h);
    }else{
      int aliases_count = 0;
      while(h->h_aliases[aliases_count] != NULL){
          if(strcmp(argv[1],h->h_aliases[aliases_count]) == 0){
             out_host(h);
             exit(EXIT_FAILURE);
          }
          aliases_count++;
      }

    }
  }
  endhostent(); 
  return 0;
}
