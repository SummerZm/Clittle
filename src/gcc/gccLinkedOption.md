## **gcc链接参数注意事项** 

### 选项--start-group archives --end-group
1. 默认的链接会根据名字先后顺序扫描一遍，可能会出现找不到定义的现象
2. 使用--start-group archives --end-group选项，会多次循环直到找不到为止.
3. 应用场景：
	编译时：A B两个.a相互调用，如果只是编译无链接, 那.a的生成可能是没有问题的
	链接时: 在.a链接中如果不使用--start-group可能会出现找不到定义的现象，使用--start-group可以避免这个现象发生

4. 原文:
	--start-group archives --end-group
	The archives should be a list of archive files. They may be either explicit file names, or -l options.
	The specified archives are searched repeatedly until no new undefined references are created. Normally, an archive is searched only once in the order that it is specified on the command line. If a symbol in that archive is needed to resolve an undefined symbol referred to by an object in an archive that appears later on the command line, the linker would not be able to resolve that reference. By grouping the archives, they all be searched repeatedly until all possible references are resolved.

	Using this option has a significant performance cost. It is best to use it only when there are unavoidable circular references between two or more archives.

5. 例子: 
	arm-linux-gcc -I/usr/local/arm/4.2.2-eabi/include -L/usr/local/arm/4.2.2-eabi/lib -static -L/work/projects/build/arm/lib -o busybox_unstripped -Wl,--sort-common -Wl,--sort-section,alignment -Wl,--start-group applets/built-in.o archival/lib.a archival/libarchive/lib.a console-tools/lib.a coreutils/lib.a coreutils/libcoreutils/lib.a debianutils/lib.a e2fsprogs/lib.a editors/lib.a findutils/lib.a init/lib.a libbb/lib.a libpwdgrp/lib.a loginutils/lib.a mailutils/lib.a miscutils/lib.a modutils/lib.a networking/lib.a networking/libiproute/lib.a networking/udhcp/lib.a printutils/lib.a procps/lib.a runit/lib.a selinux/lib.a shell/lib.a sysklogd/lib.a util-linux/lib.a util-linux/volume_id/lib.a archival/built-in.o archival/libarchive/built-in.o console-tools/built-in.o coreutils/built-in.o coreutils/libcoreutils/built-in.o debianutils/built-in.o e2fsprogs/built-in.o editors/built-in.o findutils/built-in.o init/built-in.o libbb/built-in.o libpwdgrp/built-in.o loginutils/built-in.o mailutils/built-in.o miscutils/built-in.o modutils/built-in.o networking/built-in.o networking/libiproute/built-in.o networking/udhcp/built-in.o printutils/built-in.o procps/built-in.o runit/built-in.o selinux/built-in.o shell/built-in.o sysklogd/built-in.o util-linux/built-in.o util-linux/volume_id/built-in.o -Wl,--end-group -Wl,--start-group -lcrypt -lm -Wl,--end-group
