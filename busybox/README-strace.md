## 交叉编译strace应用程序

```
1. 解压strace源码
$ tar -jxvf strace-4.5.15.tar.bz2

2. 打补丁
$ cd strace-4.5.15
$ patch -p1 < ../strace-fix-arm-bad-syscall.patch

3. 配置生成Makefile
$ ./configure --host=arm-linux CC=arm-linux-gcc

4. 执行编译
$ make

5. 复制到根文件系统
$ cp -fp strace $ROOTFS_ROOT_PATH/bin

6. 使用strace命令来跟踪测试程序的系统调用情况 
# strace -o trace.log ./leds-test.out on
```


