## 编译测试

```
0. 编译LCD内核模块
$ make && make clean
$ cp -fp lcd.ko $NFS_ROOT_PATH/rootfs/test/lcd/


1. 使用make menuconfig命令将原来的驱动程序去掉 (由原先编译进内核修改为编译成模块)
Device Drivers  --->
        Graphics support  --->
                <M> S3C2440 LCD framebuffer support


2. 编译并烧写内核
$ make uImage
$ cp -p arch/arm/boot/uImage $NFS_ROOT_PATH/rootfs/test/lcd/
$ make modules
$ cp -p drivers/video/cfbcopyarea.ko $NFS_ROOT_PATH/rootfs/test/lcd/
$ cp -p drivers/video/cfbfillrect.ko $NFS_ROOT_PATH/rootfs/test/lcd/
$ cp -p drivers/video/cfbimgblt.ko $NFS_ROOT_PATH/rootfs/test/lcd/

重启进入uboot命令行界面 (使用nfs将uImage烧写到SDRAM中，然后bootm直接启动内核即可，不必烧写到NAND中)
> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/lcd/uImage
> bootm             // 或者 bootm 30000000


3. 依次安装下列模块
# insmod cfbcopyarea.ko
# insmod cfbfillrect.ko
# insmod cfbimgblt.ko
# insmod lcd.ko

# lsmod
Module                  Size  Used by    Not tainted
lcd                     2524  1
cfbimgblt               2688  1 lcd
cfbfillrect             3552  1 lcd
cfbcopyarea             3296  1 lcd

查看framebuffer设备文件
# ls -l /dev/fb*
crw-rw----    1 0        0         29,   0 Jan  1 00:04 /dev/fb0


4. 测试lcd驱动
4.1 lcd显示测试
# echo hello > /dev/tty1    // 可以在LCD上看到hello字符串和鼠标闪烁
# cat lcd.ko > /dev/fb0     // 花屏，因为无法解析lcd.ko中的内容格式

4.2 lcd中打开sh并进行按键输入测试
修改/etc/inittab，增加以下配置行，然后重启系统，然后进入uboot重新烧写内核到SDRAM，并直接启动内核
tty1::askfirst:-/bin/sh

/dev/tty1 使用如下驱动程序：
linux/drivers/video/fbcon.c -- Low level frame buffer based console driver
这个驱动程序最终也会用到framebuffer

首先在串口终端上安装相应4个显示驱动模块和一个按键输入驱动模块(key-input.ko)，
/test/key-input # lsmod
Module                  Size  Used by    Not tainted
key_input               2656  0
lcd                     2524  1
cfbimgblt               2688  1 lcd
cfbfillrect             3552  1 lcd
cfbcopyarea             3296  1 lcd

然后按 S4 按键执行回车命令，使LCD进入sh环境
再依次按 S2 S3 S4 按键来执行 ls+Enter 命令，即可在LCD上显示当前目录内容
```


