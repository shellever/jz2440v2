## 编译根文件系统

```
$ source build/envsetup.sh
$ cd $LINUX_ARM_ROOT_PATH/output/nfsroot
$ make                  // 生成yaffs2和jffs2镜像文件
```



## 构建最小根文件系统

```
1. 创建两个设备文件：/dev/console 和 /dev/null
此设备文件无法被压缩保存或者git跟踪修改，需要自行本地创建。

查看pc主机上此两个设备文件信息
$ ls -l /dev/console /dev/null
crw------- 1 root root 5, 1 Nov 16 19:00 /dev/console
crw-rw-rw- 1 root root 1, 3 Nov 16 18:59 /dev/null

进入rootfs目录下创建设备文件
$ cd $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/
$ mkdir dev
$ cd dev
$ sudo mknod console c 5 1
$ sudo mknod null c 1 3
$ ls -l
crw-r--r-- 1 root root 5, 1 Mar 18 01:06 console
crw-r--r-- 1 root root 1, 3 Mar 18 01:06 null


2. 创建init程序 (linuxrc -> bin/busybox)
默认编译busybox安装后，生成linuxrc作为init程序，其也是指向/bin/busybox的链接


3. 创建/etc/inittab配置文件，并配置启动项，只执行 /bin/sh 交互环境
$ mkdir etc
$ cd etc
$ vi inittab
console::askfirst:-/bin/sh

此启动项askfirst会先打印出下面提示，待按回车键后激活控制台，并进入/bin/sh环境
Please press Enter to activate this console.


4. 配置文件指定的程序 (暂时不需要)


5. 安装C库 (glibc库，位于arm-linux-gcc目录下)
在开发板上只需要加载器和动态库即可。

$ mkdir lib
使用-d来保持链接文件格式++
$ cp -fpd $LINUX_ARM_ROOT_PATH/tools/gcc/arm-linux-gcc-3.4.5/arm-linux/lib/*.so* lib/
$ ls -l lib/


6. 制作文件系统镜像文件
6.1 编译制作 yaffs2 镜像文件
解压缩生成Development_util_ok目录
$ tar -jxvf yaffs_source_util_larger_small_page_nand.tar.bz2
$ cd Development_util_ok/yaffs2/utils
$ make
编译完成后，生成mkyaffs2image和mkyaffsimage两个工具。

使用mkyaffs2image命令制作fs_mini.yaffs2镜像文件
$ cd $LINUX_ARM_ROOT_PATH/output/nfsroot
// mkyaffs2image <dir> <image_file>
$ mkyaffs2image rootfs rootfs.yaffs2


6.2 编译制作 jffs2 镜像文件 (jffs2是一个压缩的文件系统)
制作 jffs2 镜像文件需要使用 mkfs.jffs2 工具，此工具由MTD设备工具包 mtd-utils-05.07.23.tar.bz2 编译生成。

此工具包又依赖于 zlib 压缩包 zlib-1.2.3.tar.gz。

编译 zlib 源码包
$ tar -zxvf zlib-1.2.3.tar.gz
$ cd zlib-1.2.3
$ ./configure --shared --prefix=/usr
$ make
$ sudo make install

编译 mkfs.jffs2
$ tar -jxvf mtd-utils-05.07.23.tar.bz2
$ cd mtd-utils-05.07.23/util
$ make
$ sudo make install


直接使用apt-get命令安装
$ apt-cache search mkfs.jffs2
mtd-utils - Memory Technology Device Utilities

$ sudo apt-get install mtd-utils


制作jffs2镜像文件
-n    表示不要在每个擦除块上都加上清除标志
-s 2048    指定一页大小为2018字节，即2K
-e 128KiB    指定一个擦除块大小为128KB
-d    指定根文件系统目录
-o    指定生成镜像文件名称
$ mkfs.jffs2 -n -s 2048 -e 128KiB -d rootfs -o rootfs.jffs2


手动设置挂载的根文件系统类型 (uboot中修改)
因为烧写后启动系统，无法自动识别根文件系统类型，无法进入系统
VFS: Mounted root (yaffs filesystem).
故需要在uboot中修改启动参数，使用 rootfstype 字段来指定根文件系统类型
uboot> printenv bootargs
bootargs=noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0
uboot> set bootargs noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0 rootfstype=jffs2
uboot> saveenv
Saving Environment to NAND...
Erasing Nand...Writing to Nand... done
uboot> printenv bootargs
bootargs=noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0 rootfstype=jffs2
uboot> boot


//
// 测试最小根文件系统
进入uboot菜单选项界面，按y来烧写yaffs2镜像文件，使用dnw烧写后，直接按b启动系统即可，不必重启uboot。
待出现下面提示时，按回车即可进入控制台。

Please press Enter to activate this console.
starting pid 763, tty '/dev/console': '/bin/sh'
# ls
bin         etc         linuxrc     sbin
dev         lib         lost+found  usr
```



## 构建增强根文件系统

```
在进入最小根文件系统中，执行ps命令，发现提示无法打开 /proc 文件错误
/ # ps
  PID  Uid        VSZ Stat Command
ps: can't open '/proc': No such file or directory



添加ps命令支持，即创建 /proc 目录
1. 创建 proc目录
$ cd ~/workspace/nfsroot/rootfs
$ mkdir proc

2. 创建启动脚本 /etc/init.d/rcS，并添加到 /etc/inittab 配置文件中
2.1 添加启动脚本到 /etc/inittab 配置文件中
$ vi etc/inittab
::sysinit:/etc/init.d/rcS

2.2 创建启动脚本 /etc/init.d/rcS 并配置
$ mkdir etc/init.d
$ vi etc/init.d/rcS
# 自动挂载proc文件系统到指定的/proc目录下
#mount -t proc none /proc
# mount -a命令会读取 /etc/fstab配置文件，并挂载里面的文件系统
mount -a

2.3 创建文件系统配置文件 /etc/fstab 并配置
$ vi etc/fstab
# device mount-point type options dump fsck order
proc /proc proc defaults 0 0
#tmpfs /tmp tmpfs defaults 0 0

2.4 添加启动脚本可执行权限
$ chmod +x etc/init.d/rcS



// 烧写并测试
# ps
  PID  Uid        VSZ Stat Command
    1 0          3092 S   init
    2 0               SW< [kthreadd]
    3 0               SWN [ksoftirqd/0]
    4 0               SW< [watchdog/0]
    5 0               SW< [events/0]
    6 0               SW< [khelper]
   55 0               SW< [kblockd/0]
   56 0               SW< [ksuspend_usbd]
   59 0               SW< [khubd]
   61 0               SW< [kseriod]
   73 0               SW  [pdflush]
   74 0               SW  [pdflush]
   75 0               SW< [kswapd0]
   76 0               SW< [aio/0]
  710 0               SW< [mtdblockd]
  745 0               SW< [kmmcd]
  766 0          3096 S   -sh
  767 0          3096 R   ps
# cat /proc/mounts          // 查看当前已挂载的文件系统
rootfs / rootfs rw 0 0
/dev/root / yaffs rw 0 0
proc /proc proc rw 0 0



// 使用 mdev 创建设备文件
// busybox-1.7.0/docs/mdev.txt
mdev 是 udev 的简化版本，它是通过读取内核信息来创建设备文件。

mdev主要有两个用途：
1. 初始化 /dev 目录
2. 动态更新
动态更新不仅是更新 /dev 目录，还支持热拔插，即接入、卸载设备时执行某些动作。

要使用 mdev，需要内核支持 sysfs 文件系统，为了减少对 Flash 的读写，还要支持 tmpfs 文件系统，
即确保内核配置CONFIG_SYSFS和 CONFIG_TMPFS两个配置项。

mdev使用命令详解
$ mount -t tmpfs mdev /dev      // 使用内存文件系统，减少对Flash的读写
$ mkdir /dev/pts                // devpts用于支持外部网络连接(telnet)的虚拟终端
$ mount -t devpts devpts /dev/pts    //
$ mount -t sysfs sysfs /sys     // mdev通过sysfs文件系统获取设备信息
$ echo /sbin/mdev > /proc/sys/kernel/hotplug    // 设置内核热插拔特性，当有设备拔插时调用/sbin/mdev程序


配置过程说明
$ cd ~/workspace/nfsroot/rootfs
$ mkdir sys dev
$ vi etc/fstab
# device mount-point type options dump fsck order
proc /proc proc defaults 0 0
sysfs /sys sysfs defaults 0 0
tmpfs /dev tmpfs defaults 0 0

$ vi etc/init.d/rcS
mount -a
mkdir /dev/pts
mount -t devpts devpts /dev/pts
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s

$ mkyaffs2image rootfs rootfs.yaffs2
uboot - y
$ sudo dnw rootfs.yaffs2
uboot - b


// 烧写并测试
// 使用mdev之前只有console和null
# ls /dev

# mount
rootfs on / type rootfs (rw)
/dev/root on / type yaffs (rw)
proc on /proc type proc (rw)
sysfs on /sys type sysfs (rw)
tmpfs on /dev type tmpfs (rw)
devpts on /dev/pts type devpts (rw)



//
// 根文件系统主要配置文件
etc/inittab        -- init初始化解析的配置文件
etc/profile        -- shell配置
etc/init.d/rcS     -- sysint初始的脚本 ，开机后自动运行的程序可以在这里设置
etc/fstab          -- rcS脚本中的 mount -a命令
```



## 挂载网络文件系统

```
挂载NFS的条件
1) 服务器允许指定目录可被挂载
2) 开发板挂载服务器指定目录


1. 从Flash上启动根文件系统，再使用mount命令挂载NFS网络文件系统
启动开发板网卡并配置IP地址
# ifconfig
# ifconfig eth0 up    // 使能eth0网卡
# ifconfig
eth0      Link encap:Ethernet  HWaddr 00:60:6E:33:44:55
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
          Interrupt:51 Base address:0xa000

# ifconfig eth0 192.168.31.17    // 配置网卡ip地址
# ifconfig
eth0      Link encap:Ethernet  HWaddr 00:60:6E:33:44:55
          inet addr:192.168.31.17  Bcast:192.168.31.255  Mask:255.255.255.0
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
          Interrupt:51 Base address:0xa000

# ping -c 3 192.168.31.212    // ping主机ip
PING 192.168.31.212 (192.168.31.212): 56 data bytes
64 bytes from 192.168.31.212: seq=0 ttl=64 time=4.700 ms
64 bytes from 192.168.31.212: seq=1 ttl=64 time=0.647 ms
64 bytes from 192.168.31.212: seq=2 ttl=64 time=0.643 ms

--- 192.168.31.212 ping statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 0.643/1.996/4.700 ms

挂载网络文件系统到指定目录下 (开发板上操作)
# mkdir -p /mnt/nfs
# mount -t nfs -o nolock 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs /mnt/nfs
# ls /mnt/nfs/
bin      etc      linuxrc  sbin     usr
dev      lib      proc     sys



2. 直接从NFS启动
uboot> printenv bootargs
bootargs=noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0

从内核的nfs参考文档中可以查找到命令行参数配置格式：
kernel/Documentation/nfsroot.txt
root=/dev/nfs
nfsroot=[<server-ip:>]<root-dir>[,<nfs-options>]
ip=<client-ip>:<server-ip>:<gw-ip>:<netmask>:<hostname>:<device>:<autoconf>


uboot> set bootargs noinitrd root=/dev/nfs nfsroot=192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs
    ip=192.168.31.17:192.168.31.212:192.168.31.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC0
uboot> saveenv
uboot> boot

// PC主机上，编写并编译生成可执行文件hello
$ cd $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs
$ vi hello.c
#include <stdio.h>
int main(void)
{
    printf("hello rootfs with nfs\n");
    return 0;
}
$ arm-linux-gcc -o hello hello.c

// 开发板上，直接运行生成的可执行文件hello
# ./hello
hello rootfs with nfs
```


