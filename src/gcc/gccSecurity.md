## **Gcc 安全编译参数**
### Linux或Android平台（后台）操作系统下，所有符合安全编译选项落地范围的产品，必须落地的选项有：ASLR（级别为2）
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



