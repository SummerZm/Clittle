
## <b>C语言小技巧/问题/常识</b> ##

### <b>1. 类型转换问题</b> ###
```C
    ....
    // utf8Word is const unsign char.
    (char)(utf8Word[i]&0xC0) != (char)0x80  // OK
    (utf8Word[i]&0xC0) != 0x80  // Error - unsgin question.
    ....
```
### <b>2. HEX 与 Ascii 转换</b> ###
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














