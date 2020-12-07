## **编译选项常见问题**

### **A. 浮点运算选项**
>  [相关链接](!https://blog.csdn.net/houxiaoni01/article/details/107521098)

- **错误: uses VFP register arguments does not.**
    1. 不支持的VFP编译：MCU不支持VFP(hard)计算
    2. 不一致的VFP编译：链接的Lib中VFP计算和App中编译VFP计算选项不一致

- **解决方案**
    1. 检查APP的编译选项 CFLAGS 的VFP的计算类型
        ```sh
            FABI:= hard # 这里把hard改为softfp
            CFLAGS += -mfloat-abi=$(FABI)
        ```

    2. 重新编译Lib库，CFLAGS中添加 -mfloat-abi=hard (与APP中编译选项一致).
    3. 更换编译器版本

- **拓展知识**
    1. 硬浮点：
    2. 软浮点：
    3. 运算参数：
        ```sh
        # -mfpu - 指定要产生那种硬件浮点运算指令. 常用的有vfp和neon等(硬件相关).
        # ARM10 and ARM9:
        -mfpu=vfp(or vfpv1 or vfpv2)
        # Cortex-A8:
        -mfpu=neon

        # -mfloat-abi=value
        ##  -mfloat-abi=soft: 调用软浮点库来支持浮点运算
        ##  常见错误：“undefined reference to '__aeabi_fdiv'”
        #
        ##  -mfloat-abi=softfp
        ##  -mfloat-abi=hard
        ##  产生硬浮点指令，至于产生哪里类型的硬浮点指令，由-mfpu指定
        #
        ##  -mfloat-abi=softfp：
        ##    生成的代码采用兼容软浮点调用接口(即使用-mfloat-abi=soft时的调用接口)
        ##    库可以采用-mfloat-abi=soft编译，而关键的应用程序可以采用-mfloat-abi=softfp来编译
        ##  
        ##  -mfloat-abi=hard
        ##    生成的代码采用硬浮点(FPU)调用接口，要求所有库和应用程序必须采用这同一个参数来编译
        #  
        ```





