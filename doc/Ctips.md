
## <b>C语言小技巧/问题/常识</b> ##

### <b>1. 类型转换问题</b> ###
```C
    ....
    // utf8Word is const unsign char.
    (char)(utf8Word[i]&0xC0) != (char)0x80  // OK
    (utf8Word[i]&0xC0) != 0x80  // Error - unsgin question.
    ....
```
### 2. HEX 与 Ascii 转换 ###
```C
    ....
    if (asciiHex>='a' && asciiHex<='f') return (unsigned char)(asciiHex-87);
    if (asciiHex>='A' && asciiHex<='F') return (unsigned char)(asciiHex-55);
    return (unsigned char)(asciiHex-'0');
    ....

    ....
    if (charHex>=0 && charHex<=9) return (unsigned char)(charHex+48);
    if (charHex>=10 && charHex<=15) return (unsigned char)(charHex+55);
    ....
    // printf   %c  打印的是ascii 码;  %2x 打印的是内存数据的Hex形式.
    // 实际上， '0'  !=  0x0
```
### <b>3. 大小端问题</b> ###
-  <b>大小端指的是数据的存储方式</b> [CPU有不同的， 大小端可能不同]
- <b>小端符合程序员的思维</b>
    ```C
        int a=0x010203;
        char* ch = &a;
        //  则 ch[0] == 03; ch[1]== 02; ch[2]==01
        //  即内存中的数据是：030201 (从低地址到高地址)
    ```
- <b>大端符合非程序员的习惯</b>
    ```C
        int a=0x010203;
        char* ch = &a;
        //  则 ch[0] == 01; ch[1]== 02; ch[2]==03
        //  即内存中的数据是：010203 (从低地址到高地址)
    ```
- <b>字节序处理</b>
    - 网络字节序: 大端字节序
    - 本机字节序: 不同主机CPU的大小端而定
    - 专门的处理函数
    ```C
        ....
        // 主机转网络 htonl();htons();
        // 网络转主机 ntohl();ntohs();
        // l => long;   s => short;
        ....
    ```
### <b>4. 范围小技巧</b> ###
-  <b>Int类型的上限</b>
    ```C
    ....
    int INT_MAX = ((unsigned)(-1))>>1;
    if (ret>INT_MAX/10 || (ret==INT_MAX/10 && start[i] >= '8')) return -1;
    ....
    ```
### <b>5. char常用处理</b> ###
- <b>HEX与半字节</b>
    
    ```C
unsigned char hex_char_merge(unsigned char chH, unsigned char chL) { return chH<<4 | chL; }
    
    void hex_char_split(unsigned char Ch, unsigned char* chH, unsigned char* chL) 
    { 
        *chH = Ch >> 4;
        *chL = Ch & 0x0F;
    }
    ```
    
- <b>Char 位操作</b>
    ```C
        ....
        // utf8-5bytes 1111 10xx 10xxxxxx[4]
        utf8Buff[0] = (unicodeVal>> 24) & 0x03 | 0xF8 ;	//  处理 1111 10xx
        utf8Buff[1] = (unicodeVal>> 18) & 0x3F | 0x80 ;	//  处理 10xx  xxxx
        // ch & 0x0F 用 & 操作清高4位
        // ch | 0x80 用 | 操作设高4位
        ....
    ```
    
- <b>存储GBK内码</b>
    ```C
        // GBK:开发人员
        char str[] = {0xbf,0xaa,0xb7,0xa2,0xc8,0xcb,0xd4,0xb1,'\0'};
    ```
    
    
    
- <b>常见的重定义错误</b>

    ```c
      // 假如以下两行代码出现在不同文件中，会产生较隐秘的重定义错误
        typedef int BOOL;
        #define BOOL int;
    ```
### <b>6. 野指针问题</b> ###  
- <b>粗心小毛病</b>
    
    ```C
        // Error - 造成指针乱飞问题 - 程序时好时坏Bug 
        char* buff[MAX_PATH];
        // Ok
        char buff[MAX_PATH];
        
    ```
### <b>7. 循坏的易错点</b>  

- **环境问题**

  ```c
  // 进程运行环境变量与其父进程有密切的关系。当进程执行shell命令失败时，可以打印其PATH环境变量(一般从父进程继承而来)
  #include <stdio.h>
  extern char** environ;
  int main()
  {
      int cursor = 0;
      for(cursor = 0; environ[cursor] != NULL; cursor++)
      {
          printf("%s\n",environ[cursor]);
      }
  }
  ```

  

- <b>细节问题：时间跃变</b>

    ```C
        // 如果系统时间发生了跃变，比如：NTP时间同步, 那么整个while()逻辑都会混乱.
        // Do something.
        time_t curr;
        time_t delay = 2;
    
        // Get time.
        time(&curr);
        while (delay>0) {
            // Do something.
            // Err1：If time jump here, the while(..) may be no exit forever.
            // Err2： You should check the return value of time().
            delay -= time(NULL) - curr;
        }
    ```

    

- <b>精度问题</b>

    ```C
        // Do something.
        time_t curr;
        time_t delay = 2;
    
        // Get time.
        time(&curr);
        while (delay>0) {
            // Do something.
            // The following two line code may be run into error when the time cost of this loop is too short.
            // time()函数是秒精度单位级别的，如果while循环花费的时间不足1秒，那么time(NULL)-curr恒等于0, 循环永远不退出。
            delay -= time(NULL) - curr;
            time(&curr);
        }
    ```

### <b>8. 无损/合理拓展代码</b>
- <b>无损添加逻辑</b>
    ```c
        // 问题1：upgrade_check()返回值不明确，无法区分升级失败和升级文件不存在
        // 问题2：upgrade_check()除了升级还耦合目录损坏校验等其他逻辑
        // 已知：如果存在升级文件，则升级函数upgrade_check()返回值可近似当成升级状态
        // 拓展：能够识别升级失败，创建升级失败标志文件
        static BOOL _checkUpgrade() {
            BOOL isUpgradeSuccessful=FALSE;
            if(SS_OK==upgrade_check()){
                isUpgradeSuccessful=TRUE;
            }
            return isUpgradeSuccessful;
        }

        /* 错误的改法 */
        // 改变了代码执行流程.
        // 修改前：一定会执行upgrade_check().
        // 修改后：当UPGRADE_FULL_PATH文件存在时才会执行upgrade_check().
        // 如果upgrade_check()做了升级之外的工作，那么这个改动就会爆炸.
        static BOOL _checkUpgrade(){
            BOOL isUpgradeSuccessful=FALSE;
            if (existfile(UPGRADE_FULL_PATH)) {
                if(SS_OK==upgrade_check()){
                    isUpgradeSuccessful=TRUE;
                }
                else {
                    char failPidFile[MAX_PATH];
                    PATH_PID(failPidFile, SOTFWARE_UPGRADE_FAIL_PID);
                    createPidFile(failPidFile);
                }
            }
            return isUpgradeSuccessful;
        }

        /* 拓展后代码 */
        // 原则：对于实现不明的接口，只增不减，尽量避免修改原代码流程
        static BOOL _checkUpgrade(){
            char failPidFile[MAX_PATH];
            // 严谨1：是否有升级文件【必须将当前状态存起来：保证函数内部状态的一致性，不要多处多次获取】
            BOOL isNeedUpgrade = existfile(UPGRADE_FULL_PATH);
            BOOL isUpgradeSuccessful=FALSE;
            PATH_PID(failPidFile, SOTFWARE_UPGRADE_FAIL_PID);
            // 严谨2：如果有残留的标志文件，则清除
            if (isNeedUpgrade && existfile(failPidFile)) {
                deletefile(failPidFile);
            }
            // 严谨3：上面拓展的代码并不会影响upgrade_check()执行
            if(SS_OK==upgrade_check()){
                isUpgradeSuccessful=TRUE;
            }
            else {
                // 呼应严谨1：这里的必须使用isNeedUpgrade，不能用existfile(UPGRADE_FULL_PATH)，因为上面存在deletefile操作。
                // 保证函数内部状态的一致性
                if (isNeedUpgrade && !existfile(failPidFile)) { createPidFile(failPidFile); }
            }
            return isUpgradeSuccessful;
        }
    ```

### <b>9. 程序运行过程中地址偏移</b>
- <b>问题: 程序链接静态库的时，在程序和静态库中打印同一全局动态分配结构体成员的地址,结果显示不同, 发生了偏移.</b>
  <b>原因: 编译静态库时用的头文件和编译程序时用的头文件不同。比如：头文件中结构体某个成员Buffer增加字节</b>













