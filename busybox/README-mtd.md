## 移植mtd

```
1. 下载mtd源码包
ftp://ftp.infradead.org/pub/mtd-utils/
http://www.linux-mtd.infradead.org/


2. 解压并进入源码目录
$ tar -jxvf mtd-utils-05.07.23.tar.bz2
$ cd mtd-utils-05.07.23/util


3. 指定只编译生成flash_erase和flash_eraseall两个工具
$ make CROSS=arm-linux- flash_erase
$ make CROSS=arm-linux- flash_eraseall


4. 安装flash_erase等工具到指定目录下
$ export MTD_UTIL_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/mtd/mtd-utils-05.07.23/util
$ mkdir -p $MTD_UTIL_INSTALL_DIR
$ cp -p flash_erase flash_eraseall $MTD_UTIL_INSTALL_DIR


5. 查看安装的内容
$ ls $MTD_UTIL_INSTALL_DIR


6. 复制mtd编译生成的内容到开发板根文件系统中
$ cp -fdpr $MTD_UTIL_INSTALL_DIR/* $ROOTFS_ROOT_PATH/sbin/


7. 格式化分区及挂载
擦除指定分区，默认格式化文件系统为yaffs
# flash_eraseall /dev/mtd3
挂载格式化后，已有文件系统的分区
# mount -t yaffs /dev/mtdblock3 /mnt
```


