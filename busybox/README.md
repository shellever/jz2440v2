## busybox编译

```
0. 解压源码，查看编译说明
$ tar -jxvf busybox-1.7.0.tar.bz2
$ cd busybox-1.7.0
$ vi INSTALL

1. 手工配置 Busybox (其中++为手动开启选项，其余为默认配置)
$ make menuconfig
Busybox Settings  --->
    Build Options  --->
        [] Build BusyBox as a static binary (no shared libs)    // 指定是否使用静态连接
        (arm-linux-) Cross Compiler prefix    // 配置交叉工具链命令前缀，在busybox-1.7.0没有此选项
    Busybox Library Tuning  --->
        [*] Tab completion   // tab键命令补全，默认未打开++
        [*] Fancy shell prompts      // 使在profile中设置的命令提示符PS1中的转义字符生效++
Applets ---
Init Utilities  --->
    [*] Support reading an inittab file     // 使/etc/inittab配置文件被busybox的init进程解析
Linux System Utilities  --->        // 默认选择
    [*] mdev
    [*] Support /etc/mdev.conf
    [*] Support command execution at device addition/removal
    [*] mount
    [*] Support mounting NFS file systems
    [*] umount
    [*] Support option -a


2. 编译源码
2.1 配置架构和交叉编译命令前缀
2.1.1 使用export命令导出变量
$ export ARCH=arm
$ export CROSS_COMPILE=arm-linux-
2.1.2 直接修改Makefile文件中指定变量 (++)
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-
2.2 开始编译
$ make                  

2.3 直接make命令中指定编译架构和交叉编译工具链前缀
$ make ARCH=arm CROSS_COMPILE=arm-linux-


3. 安装到指定路径下 (使用nfs可以挂载此目录下的根文件系统)
$ mkdir -p ~/workspace/nfsroot/rootfs
$ make CONFIG_PREFIX=~/workspace/nfsroot/rootfs install


4. 查看安装后的根文件系统
$ ls -l ~/workspace/nfsroot/rootfs/
total 12
drwxrwxr-x 2 linuxfor linuxfor 4096 Nov 18 23:49 bin
lrwxrwxrwx 1 linuxfor linuxfor   11 Nov 18 23:49 linuxrc -> bin/busybox
drwxrwxr-x 2 linuxfor linuxfor 4096 Nov 18 23:49 sbin
drwxrwxr-x 4 linuxfor linuxfor 4096 Nov 18 23:49 usr
```

