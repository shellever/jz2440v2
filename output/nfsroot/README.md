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
使用dnw烧写后，直接按b启动系统即可，不必重启uboot。
待出现下面提示时，按回车即可进入控制台。

Please press Enter to activate this console.
starting pid 763, tty '/dev/console': '/bin/sh'
# ls
bin         etc         linuxrc     sbin
dev         lib         lost+found  usr
```


