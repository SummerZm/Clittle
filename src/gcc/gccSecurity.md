## **Gcc 安全编译参数**
### **A. Linux或Android平台（后台）操作系统下，所有符合安全编译选项落地范围的产品，必须落地的选项有：ASLR（级别为2）**
- **问题背景**
    ```sh
    # 目的：可以通过猜测，运行程序的地址进行破解
    # 做法：支持程序随机加载至指定位置
    # 方案1：操作系统ASLR功能选项。【局限】只能对stack,heap,librarise进行随机化
    # 方案2：ASLR + PIE 【功能】在ASLR 支持对代码段，全局区地址进行随机化。
    #           
    ```

- **ASLR的是操作系统的功能选项** 作用于executable（ELF）装入内存运行时，因而只能随机化stack、heap、libraries的基址。
- **ASLR有0/1/2三种级别** 0：ASLR未开启，1：随机化stack、libraries，2：还会随机化heap。
- **ASLR与PIE，PIC的关系**
    1. ASLR 是操作系统的设置项。
    2. PIC是编译动态库随机化地址的设置项。PIE是编译可以执行程序时随机随机化地址的设置项。
    3. PIE 编译处理的可执行程序像是一个特殊的SO，支持随机加载。
    4. PIE 赋予了可执行程序随机加载的能力；ASLR 操作系统设置项决定了是否使用该能力。
    5. ASLR 与 PIE 配合使用
        ```sh
        # ASLR[0] + PIE : 不进行随机化
        # ASLR[1] + PIE : Stack, Heap, libraries, executable-base [被动随机+受heap随机化的影响][heap和executable-base是连在一起的]
        # ASLR[2] + PIE : Stack, Heap, libraries, executable-base 随机化
        ```

- **相关操作**
    1. 设置 ASLR： echo 1 > /proc/sys/kernel/randomize_va_space

### **B. 栈保护(-fstack-protector-all/-fstack-protector-strong)**
- **程序压栈汇编代码是什么？**
    ```C
    // 在call执行后，在被调用函数最开始进行的。目的是保存上一个函数的栈信息，用于返回执行。
    push %rbp       /* 将当前函数栈底指针入栈 */
    mov %rsp, %rbp，/* 然后将栈顶指针赋给栈底指针 */ 
    ```
- **栈保护实现简介**
    1. gcc堆栈保护技术基本都是通过canaries探测来实现的
    2. Canaries探测要检测对函数栈的破坏，需要修改函数栈的组织， 要在缓冲区和控制信息(压在栈中的函数返回后的RBP和RIP)中间插一个canary word
    3. 当缓冲区被破坏的时候，canary word会在函数栈控制信息被破坏之前被破坏
    4. 通过检测canary word的值是否被修改，就可以判断出是否发生溢出攻击。

- **为什么要开启栈对齐，它的编译选项是？**
    1. 栈保护功能需要插入8bytes的数据。不同指令集决定了硬件寄存器的位数实现，决定了对齐的方式。
    2. 编译选项：-mpreferred-stack-boundary=3。 3表示2的3次方。即：8位对齐。 
    3. 栈保护开启导致栈对齐的历史问题
        ```sh
        #   Intel在Pentium III推出了SSE指令集，SSE 加入新的 8 个128Bit(16bytes)寄存器（XMM0～XMM7）。
        #   最初的时候，这些寄存器智能用来做单精度浮点数计算
        #   自从SSE2开始，这些寄存器可以被用来计算任何基本数据类型的数据了。
        #   往XMM0～XMM7里存放数据，是以16字节为单位，所以呢 内存变量首地址必须要对齐16字节，
        #   否则会引起CPU异常，导致指令执行失败。所以这就是gcc默认采用16bytes进行栈对齐的原因。
        # 
        # 在SSE扩展被关闭时，-mpreferred-stack-boundary参数值是可以修改的。但是，但是，但是，当该选项值被修改后，编译链接16bytes栈对齐的库时，会导致错误。
        ```

    4. **重点/易错点**
        ```sh
        # 在SSE扩展被关闭时，-mpreferred-stack-boundary参数值是可以修改的。但是，当该选项值被修改后，编译链接16bytes栈对齐的库时，会导致错误。
        ```

- **开启栈保护将导致什么问题？**
    1. **问题：链接第三方库时链接错误**
    2. **原因分析**
        ```sh
        # 直接原因：对齐问题导致的不兼容
        # 历史原因：指令集决定了gcc的编译方式
        # 根本原因：栈保护功能需要插入8bytes的数据，造成对齐问题
        ```
    
- **栈保护gcc演变历史**
> **GCC 4.1 堆栈保护才被加入，所采用的堆栈保护实现Stack-smashing Protection（SSP，又称 ProPolice）。到目前GCC6.2中与堆栈保护有关的编译选项，有如下几个**
1. -fstack-protector: 启用堆栈保护，不过只为局部变量中含有 char 数组的函数插入保护代码。
2. -fstack-protector-all:  启用堆栈保护，为所有函数插入保护代码。
3. -fstack-protector-strong(GCC4.9引入)
    ```sh
    # 包含和-fstack-protector一样的功能
    # 包含额外的保护：函数内部有数组定义，以及有对局部栈上的地址引用。
    ```
4. -fstack-protector-explicit： 和-fstack-protector的区别是：只对有stack_protect属性的函数进行保
5. -fno-stack-protector： 禁用堆栈保护。

- **栈保护图解**  
![./image/stack_protect.png](./image/stack_protect.png)  
![./image/stack_protect_2.png](./image/stack_protect_2.png)  

