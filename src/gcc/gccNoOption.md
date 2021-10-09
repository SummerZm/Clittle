## GCC no option ##
- no-asm: 在交叉编译过程中不使用汇编代码代码加速编译过程.原因是它的汇编代码是对arm格式不支持的。
- no-async: 交叉编译工具链没有提供GNU C的ucontext库.
- 交叉编译时 报错 -m64， 可以将其从makefile 中移除 