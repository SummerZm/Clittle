#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>


int translateHexToChars(char* src, char* dst, int slen, int dlen);

int get_file_lines(char * file_path)
{
  FILE *fp;
  int flag = 0, file_row = 0, count = 0;
  if((fp = fopen(file_path, "r")) == NULL)
    return -1;
  while(!feof(fp))
  {
    flag = fgetc(fp);
    if(flag == '\n')
      count++;
  }
  file_row = count + 1; 
  printf("row = %d\n", file_row);
  fclose(fp);
  return file_row;
}

void read_speacial_line(char * file_path,int i,char * str) 
{ 
    FILE *fp; 
    int which_line=i;            
    int current_index=0;                        
    if((fp = fopen(file_path,"r")) == NULL) 
    { 
        printf("error!"); 
        return ; 
    } 
 
    while (!feof(fp)) 
    { 
    
        if (current_index==which_line)
        {
            fgets(str,1024,fp); 
            printf("1=%s", str);  
            break;  
        }
        fgets(str,1024,fp);  
        current_index++;
 
    } 
    fclose(fp);                    
}

int main()
{

   char * file_path="upgrade.hex";
   int lines=get_file_lines(file_path);
   char * str =malloc(1024);
   if (str)  
   {  
      memset(str,0,1024); 
   }

   read_speacial_line(file_path,lines-5,str); 
   char version[9]={'\0'};
   char charVersion[256];
   memset(charVersion, 0 , sizeof(charVersion));
   strncpy(version,str +17,8);
   printf("version=%s\n",version);
   translateHexToChars(version, charVersion, 8, 4);
   printf("charversion=%d-%d-%d-%d\n", charVersion[0], charVersion[1], charVersion[2], charVersion[3]);
   free(str);
   str=NULL;
   return 0;
   
}
