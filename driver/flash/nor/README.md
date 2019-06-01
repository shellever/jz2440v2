## 编译测试

```
// 使用UBOOT体验NOR FLASH的操作(开发板设为NOR启动，进入UBOOT)
Reset：复位，往“任何地址”写入“F0H”便是复位了。

解锁命令：555H 2AAH

1. 读数据 (可以像内存一样直接读)
Boogie> md.b 0                                                                 
00000000: 17 00 00 ea 14 f0 9f e5 14 f0 9f e5 14 f0 9f e5    ................ 

格式化输出文件的前10个字节，并以16进制显示
Curdir: ~/workspace/jz2440/driver/flash/nor
linuxfor@pc-u16-d64$ hexdump -C ../../../uboot/u-boot-1.1.6/u-boot.bin -n 10
00000000  17 00 00 ea 14 f0 9f e5  14 f0                    |..........|  


2. 读ID (需要退出读ID状态后才能重新读数据)  - Read Silicon ID
NOR手册上:
往地址555H写AAH
往地址2AAH写55H
往地址555H写90H
读0地址得到厂家ID: C2H
读1地址得到设备ID: 22DAH或225BH
退出读ID状态(即复位): 给任意地址写F0H

2440的A1接到NOR的A0，所以2440发出(555H<<1), NOR才能收到555H这个地址
UBOOT怎么操作？

往地址AAAH写AAH                      mw.w aaa aa
往地址554写55H                       mw.w 554 55
往地址AAAH写90H                      mw.w aaa 90
读0地址得到厂家ID: C2H               md.w 0 1
读2地址得到设备ID: 22DAH或225BH      md.w 2 1
退出读ID状态:                        mw.w 0 f0

Boogie> mw.w aaa aa                                                          
Boogie> mw.w 554 55                                                          
Boogie> mw.w aaa 90                                                          
Boogie> md.w 0 1                                                             
00000000: 00c2    ..                                                         
Boogie> md.w 2 1                  // md.w <起始地址>  <读取多少个字>                                          
00000002: 2249    I"                                                         
Boogie> mw.w 0 f0


3. NOR有两种规范, jedec, cfi(common flash interface) - CFI Query
   读取CFI信息

NOR手册：  
进入CFI模式    往55H写入98H
读数据:        读10H得到0051
               读11H得到0052
               读12H得到0059
               读27H得到容量

2440的A1接到NOR的A0，所以2440发出(555H<<1), NOR才能收到555H这个地址
UBOOT怎么操作？
进入CFI模式    往AAH写入98H            mw.w aa 98
读数据:        读20H得到0051           md.w 20 1
               读22H得到0052           md.w 22 1
               读24H得到0059           md.w 24 1
               读4EH得到容量           md.w 4e 1
               退出CFI模式             mw.w 0 f0

Boogie> mw.w aa 98                                                           
Boogie> md.w 20 1                                                            
00000020: 0051    Q.                                                         
Boogie> md.w 22 1                                                            
00000022: 0052    R.                                                         
Boogie> md.w 24 1                                                            
00000024: 0059    Y.                                                         
Boogie> md.w 4e 1                                                             
0000004e: 0015    ..                     // 容量大小为 2^21 = 2 * 2^10 * 2^10 = 2Mbytes                                       
Boogie> mw.w 0 f0


4. 写数据: 在地址0x100000写入0x1234 (uboot大小没有达到1M，故为了不破坏uboot，在0x100000地址处进行测试) - Program
md.w 100000 1     // 得到ffff
mw.w 100000 1234
md.w 100000 1     // 还是ffff

// Nor无法像内存一样直接写
Boogie> md.w 100000 1                                                        
00100000: ffff    ..                                                         
Boogie> mw.w 100000 1234                                                     
Boogie> md.w 100000 1                                                        
00100000: ffff    ..


NOR手册：
往地址555H写AAH
往地址2AAH写55H
往地址555H写A0H
往地址PA写PD

2440的A1接到NOR的A0，所以2440发出(555h<<1), NOR才能收到555h这个地址
UBOOT怎么操作？
往地址AAAH写AAH               mw.w aaa aa
往地址554H写55H               mw.w 554 55
往地址AAAH写A0H               mw.w aaa a0
往地址0x100000写1234h         mw.w 100000 1234

写之前需要先擦除，以保证内容为0xFF

Boogie> mw.w aaa aa                                                          
Boogie> mw.w 554 55                                                          
Boogie> mw.w aaa a0                                                          
Boogie> mw.w 100000 1234                                                     
Boogie> mw.w 0 f0                                                            
Boogie> md.w 100000 1                                                        
00100000: 1234    4.                  // 写入成功


测试1: 通过配置内核支持NOR FLASH
1. 配置内核编译参数 ，开启nor模块编译 (由于在nand驱动程序测试中已经开启nand分区，所以需要先nfs nand/uImage并bootm后再进行nor驱动安装测试)
$ make menuconfig
Device Drivers  --->
    <*> Memory Technology Device (MTD) support
        Mapping drivers for chip access  --->
            <M> CFI Flash device in physical memory map     // 按M键编译成内核模块，然后才会出现下面三个配置选项
            (0x0) Physical start address of flash mapping   // 物理基地址
            (0x00200000) Physical length of flash mapping   // 地址长度
            (2) Bank width in octets                        // 位宽(八进制) 8 x 2 = 16 bits

2. 编译内核模块
$ make modules
$ cp -fp drivers/mtd/maps/physmap.ko ~/workspace/nfsroot/rootfs/test/nor/

3. 启动开发板 (nor启动)
# ls -l /dev/mtd*
ls: /dev/mtd*: No such file or directory

# insmod physmap.ko
physmap platform flash device: 00200000 at 00000000
physmap-flash.0: Found 1 x16 devices at 0x0 in 16-bit bank
Amd/Fujitsu Extended Query Table at 0x0040
number of CFI chips: 1
cfi_cmdset_0002: Disabling erase-suspend-program due to code brokenness.
cmdlinepart partition parsing not available
RedBoot partition parsing not available

# ls -l /dev/mtd*
crw-rw----    1 0        0         90,   0 Jan  1 00:00 /dev/mtd0
crw-rw----    1 0        0         90,   1 Jan  1 00:00 /dev/mtd0ro
brw-rw----    1 0        0         31,   0 Jan  1 00:00 /dev/mtdblock0

# cat /proc/mtd
dev:    size   erasesize  name
mtd0: 00200000 00010000 "physmap-flash.0"      // size 大小是 2M，擦除块的大小是 64K

# rmmod physmap
Device 'physmap-flash.0' does not have a release() function, it is broken and m.
WARNING: at drivers/base/core.c:107 device_release()    // 警告，原来内核自带的驱动程序中没有release()函数
[<c002ede8>] (dump_stack+0x0/0x14) from [<c01b743c>] (device_release+0x84/0x98)
[<c01b73b8>] (device_release+0x0/0x98) from [<c0177714>] (kobject_cleanup+0x68/)
[<c01776ac>] (kobject_cleanup+0x0/0x80) from [<c0177740>] (kobject_release+0x14)
r7:00000000 r6:c3c50000 r5:c017772c r4:bf000f2c
[<c017772c>] (kobject_release+0x0/0x18) from [<c01784d0>] (kref_put+0x8c/0xa4)
[<c0178444>] (kref_put+0x0/0xa4) from [<c01776a4>] (kobject_put+0x20/0x28)
r5:bf001060 r4:bf000ea4
[<c0177684>] (kobject_put+0x0/0x28) from [<c01b7b44>] (put_device+0x1c/0x20)
[<c01b7b28>] (put_device+0x0/0x20) from [<c01bc25c>] (platform_device_put+0x1c/)
[<c01bc240>] (platform_device_put+0x0/0x20) from [<c01bc584>] (platform_device_)
[<c01bc568>] (platform_device_unregister+0x0/0x20) from [<bf000400>] (physmap_e)
r4:c035d014
[<bf0003ec>] (physmap_exit+0x0/0x28 [physmap]) from [<c00610e4>] (sys_delete_mo)
[<c0060ed0>] (sys_delete_module+0x0/0x29c) from [<c002aea0>] (ret_fast_syscall+)
r8:c002b044 r7:00000081 r6:000a00a8 r5:bee3cec4 r4:000a1548


测试2: 使用自己写的驱动程序 (一定要在nor启动下挂载才行，因为2440使用nand启动时，是访问不了nor的前4k地址)
1. ls /dev/mtd*
2. insmod s3c_nor.ko
3. ls /dev/mtd*
4. 格式化: flash_eraseall -j /dev/mtd1
5. mount -t jffs2 /dev/mtdblock1 /mnt
   在/mnt目录下操作文件


# ls -l /dev/mtd*
ls: /dev/mtd*: No such file or directory
 
# insmod nor.ko                                                      
use cfi_probe                                                                  
s3c_nor: Found 1 x16 devices at 0x0 in 16-bit bank                             
Amd/Fujitsu Extended Query Table at 0x0040                                    
number of CFI chips: 1                                                         
cfi_cmdset_0002: Disabling erase-suspend-program due to code brokenness.       
Creating 2 MTD partitions on "s3c_nor":                                        
0x00000000-0x00040000 : "bootloader_nor"                                       
0x00040000-0x00200000 : "root_nor"                      
                       
# ls -l /dev/mtd*                                                    
crw-rw----    1 0        0         90,   0 Jan  1 00:09 /dev/mtd0              
crw-rw----    1 0        0         90,   1 Jan  1 00:09 /dev/mtd0ro            
crw-rw----    1 0        0         90,   2 Jan  1 00:09 /dev/mtd1              
crw-rw----    1 0        0         90,   3 Jan  1 00:09 /dev/mtd1ro            
brw-rw----    1 0        0         31,   0 Jan  1 00:09 /dev/mtdblock0         
brw-rw----    1 0        0         31,   1 Jan  1 00:09 /dev/mtdblock1 

使用flash_eraseall来格式化/dev/mtd1分区，默认文件系统为jffs2
# flash_eraseall -j /dev/mtd1                                
Erasing 64 Kibyte @ 1b0000 -- 96 % complete. Cleanmarker written at 1b0000.    

挂载nor flash上的分区到/mnt目录下
# mount -t jffs2 /dev/mtdblock1 /mnt       

在挂载目录下创建文件，并查看内容                           
# echo "hello nor flash" > /mnt/firstme.txt                          
# cat /mnt/firstme.txt                                               
hello nor flash 


查看flash_eraseall工具帮助
# flash_eraseall --help                                      
Usage: ../nand/flash_eraseall [OPTION] MTD_DEVICE                              
Erases all of the specified MTD device.                                        

  -j, --jffs2    format the device for jffs2                                   
  -q, --quiet    don't display progress messages                               
      --silent   same as --quiet                                               
      --help     display this help and exit                                    
      --version  output version information and exit



// 用内存模拟一个 NOR FLASH
内核自带的使用内存来模拟flash，通过memcopy()等来实现堆内存读写擦除
drivers/mtd/devices/mtdram.c
```


