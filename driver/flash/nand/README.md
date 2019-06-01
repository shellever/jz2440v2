## 编译测试

```
用UBOOT来体验NAND FLASH的操作
1. 读ID
uboot> help md
md [.b, .w, .l] address [# of objects]
    - memory display

uboot> md.l 0x4E000004 1            //
4e000004: 00000003    ....
uboot> mw.l 0x4E000004  1           // NFCONT的bit1设为0  选中
uboot> mw.b 0x4E000008 0x90         // NFCMMD=0x90        发出命令0x90
uboot> mw.b 0x4E00000C 0x00         // NFADDR=0x00        发出地址0x00
uboot> md.b 0x4E000010 1            // val=NFDATA         读1个字节数据得到0xEC
4e000010: ec    .
uboot> md.b 0x4E000010 1            // val=NFDATA         读1个字节数据得到0xDA
4e000010: da    .
uboot> mw.b 0x4E000008 0xFF         // NFCMMD=0xff        退出读ID的状态
uboot>

2. 读内容: 读0地址的数据 (估计不能使用网络文件系统，否则会读出来全部是FF)
使用UBOOT命令:
nand dump 0
Page 00000000 dump:
        17 00 00 ea 14 f0 9f e5  14 f0 9f e5 14 f0 9f e5

操作               S3C2440             u-boot
选中               NFCONT的bit1设为0   md.l 0x4E000004 1; mw.l 0x4E000004  1
发出命令0x00       NFCMMD=0x00         mw.b 0x4E000008 0x00
发出地址0x00       NFADDR=0x00         mw.b 0x4E00000C 0x00
发出地址0x00       NFADDR=0x00         mw.b 0x4E00000C 0x00
发出地址0x00       NFADDR=0x00         mw.b 0x4E00000C 0x00
发出地址0x00       NFADDR=0x00         mw.b 0x4E00000C 0x00
发出地址0x00       NFADDR=0x00         mw.b 0x4E00000C 0x00
发出命令0x30       NFCMMD=0x30         mw.b 0x4E000008 0x30
读数据得到0x17     val=NFDATA          md.b 0x4E000010 1
读数据得到0x00     val=NFDATA          md.b 0x4E000010 1
读数据得到0x00     val=NFDATA          md.b 0x4E000010 1
读数据得到0xea     val=NFDATA          md.b 0x4E000010 1
退出读状态         NFCMMD=0xff         mw.b 0x4E000008 0xff



//
// NAND 驱动测试
3. 只添加nand_scan()扫描函数，未添加add_mtd_partitions()添加分区函数
此时做实验可以不卸载原来的 NAND 驱动
# insmod nand.ko
NAND device: Manufacturer ID: 0xec, Chip ID: 0xda (Samsung NAND 256MiB 3,3V 8-b)
Scanning device for bad blocks
Bad eraseblock 437 at 0x036a0000
Bad eraseblock 1029 at 0x080a0000
Bad eraseblock 1038 at 0x081c0000
Bad eraseblock 1090 at 0x08840000
Bad eraseblock 1685 at 0x0d2a0000
Bad eraseblock 1936 at 0x0f200000
Bad eraseblock 1951 at 0x0f3e0000



4. NAND Flash格式化即挂载测试
4.1 去掉内核自带的NAND FLASH驱动
$ make menuconfig
-> Device Drivers
  -> Memory Technology Device (MTD) support
    -> NAND Device Support
   < >   NAND Flash support for S3C2410/S3C2440 SoC    // 按n键即可取消

4.2 使用新内核启动, 并且使用NFS作为根文件系统
$ make uImage

重启进入uboot命令行界面 (使用nfs将uImage烧写到SDRAM中，然后启动即可，不必烧写到NAND中)
> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/flash/nand/uImage
> bootm 30000000

4.3 安装nand驱动，并查看设备信息
# insmod nand.ko
NAND device: Manufacturer ID: 0xec, Chip ID: 0xda (Samsung NAND 256MiB 3,3V 8-bit)
Scanning device for bad blocks
Bad eraseblock 437 at 0x036a0000
Bad eraseblock 1029 at 0x080a0000
Bad eraseblock 1038 at 0x081c0000
Bad eraseblock 1090 at 0x08840000
Bad eraseblock 1685 at 0x0d2a0000
Bad eraseblock 1936 at 0x0f200000
Bad eraseblock 1951 at 0x0f3e0000
Creating 4 MTD partitions on "NAND 256MiB 3,3V 8-bit":
0x00000000-0x00040000 : "bootloader"
0x00040000-0x00060000 : "params"
0x00060000-0x00260000 : "kernel"
0x00260000-0x10000000 : "root"

# ls -l /dev/mtd*
crw-rw----    1 0        0         90,   0 Jan  1 00:02 /dev/mtd0
crw-rw----    1 0        0         90,   1 Jan  1 00:02 /dev/mtd0ro
crw-rw----    1 0        0         90,   2 Jan  1 00:02 /dev/mtd1
crw-rw----    1 0        0         90,   3 Jan  1 00:02 /dev/mtd1ro
crw-rw----    1 0        0         90,   4 Jan  1 00:02 /dev/mtd2
crw-rw----    1 0        0         90,   5 Jan  1 00:02 /dev/mtd2ro
crw-rw----    1 0        0         90,   6 Jan  1 00:02 /dev/mtd3
crw-rw----    1 0        0         90,   7 Jan  1 00:02 /dev/mtd3ro
brw-rw----    1 0        0         31,   0 Jan  1 00:02 /dev/mtdblock0
brw-rw----    1 0        0         31,   1 Jan  1 00:02 /dev/mtdblock1
brw-rw----    1 0        0         31,   2 Jan  1 00:02 /dev/mtdblock2
brw-rw----    1 0        0         31,   3 Jan  1 00:02 /dev/mtdblock3

4.4 挂载已有文件系统的分区
# mkdir /mnt
# mount /dev/mtdblock3 /mnt
UDF-fs: No VRS found
yaffs: dev is 32505859 name is "mtdblock3"
yaffs: passed flags ""
yaffs: Attempting MTD mount on 31.3, "mtdblock3"
yaffs: auto selecting yaffs2
block 419 is bad
block 1011 is bad
block 1020 is bad
block 1072 is bad
block 1667 is bad
block 1918 is bad
block 1933 is bad
# ls -l /mnt
drw-rw-rw-    1 0        0            2048 Jan  1 00:10 lost+found
# umount /mnt
save exit: isCheckpointed 1


4.5 格式化并挂载
编译格式化工具flash_eraseall
flash_erase - 擦除一个扇区
flash_eraseall - 擦除整个分区
$ tar -jxvf mtd-utils-05.07.23.tar.bz2
$ cd mtd-utils-05.07.23/util
$ vi Makefile
CROSS=arm-linux-
$ make
$ cp -fp flash_erase flash_eraseall ~/workspace/jz2440v2/output/nfsroot/rootfs/test/flash/nand/

擦除指定分区(/dev/mtd3为root分区)，默认格式化文件系统为yaffs
# ./flash_eraseall /dev/mtd3
Erasing 128 Kibyte @ 3420000 -- 20 % complete.
Skipping bad block at 0x03440000
Erasing 128 Kibyte @ 7e20000 -- 49 % complete.
Skipping bad block at 0x07e40000
Erasing 128 Kibyte @ 7f40000 -- 50 % complete.
Skipping bad block at 0x07f60000
Erasing 128 Kibyte @ 85c0000 -- 52 % complete.
Skipping bad block at 0x085e0000
Erasing 128 Kibyte @ d020000 -- 82 % complete.
Skipping bad block at 0x0d040000
Erasing 128 Kibyte @ ef80000 -- 94 % complete.
Skipping bad block at 0x0efa0000
Erasing 128 Kibyte @ f160000 -- 95 % complete.
Skipping bad block at 0x0f180000
Erasing 128 Kibyte @ fd80000 -- 99 % complete.


挂载格式化后，已有文件系统的分区
# mount -t yaffs /dev/mtdblock3 /mnt
yaffs: dev is 32505859 name is "mtdblock3"
yaffs: passed flags ""
yaffs: Attempting MTD mount on 31.3, "mtdblock3"
yaffs: auto selecting yaffs2
block 419 is bad
block 1011 is bad
block 1020 is bad
block 1072 is bad
block 1667 is bad
block 1918 is bad
block 1933 is bad
# ls -l /mnt
drw-rw-rw-    1 0        0            2048 Jan  1 00:18 lost+found


4.6 在/mnt目录下建文件  
# echo "hello world" > /mnt/firstme.txt
# umount /mnt
# reboot
> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/flash/nand/uImage
> bootm 30000000
# insmod nand.ko
# mount -t yaffs /dev/mtdblock3 /mnt
# cat /mnt/firstme.txt
# umount /mnt
```


