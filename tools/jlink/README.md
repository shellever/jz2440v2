## JLink下载与安装

```
SEGGER - JLink
https://www.segger.com/downloads/jlink
J-Link Software and Documentation Pack[Beta version]/
J-Link Software and Documentation pack for Linux, DEB installer, 64-bit [Beta]
https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPackBeta


installation
1. extract
$ tar -zxvf JLink_Linux_V635g_x86_64.tgz

2. read doc
$ vi README.txt

3. install
$ cd JLink_Linux_V635g_x86_64
$ sudo cp -p 99-jlink.rules /etc/udev/rules.d/
$ cp -Rp./* ~/devsoft/tools/jlink/JLink_Linux_V635g_x86_64

4. testing
$ ./JLinkExe
```


## 使用JLink烧写说明

s3c2440支持两种启动方式：
1. 以nand方式启动，此时nand中的前4k内容会由硬件自动复制到片内SRAM的4k空间中，
片内4k内存会被映射到bank0，起始地址为`0`处。
2. 以nor方式启动，此时nor会也会被映射到bank0，并且nor可以像内存一样被直接读取，
cpu从nor的0地址开始运行，同时片内SRAM会被映射到`0x40000000`地址处。

在一开始Flash没有烧写uboot前，可以使用JLink将bin文件烧写到sram中，因此可以在sram执行一个初始化sdram的程序，
然后再使用JLink将程序烧写到sdram中。


#### 0. 编译初始化sdram的init程序

```
$ source build/envsetup.sh
$ cd $LINUX_ARM_ROOT_PATH/hardware/init
$ make
```

默认编译生成两个bin文件，其中init_nand.bin用于烧写Nand时使用，init_nor.bin用于烧写Nor时使用，两者区别在于链接地址不同。


#### 1. 使用JLink间接烧写Nand Flash步骤

```
1. 查看JLink的usb设备是否识别成功 (尝试拔插即可)
$ lsusb
Bus 001 Device 010: ID 1366:0101 SEGGER J-Link PLUS


2. 以Nand方式对开发板进行上电


3. 进入J-Link命令行环境 (自动识别设备id)
$ JLinkExe
SEGGER J-Link Commander V5.00e ('?' for help)
Compiled Jun 23 2015 18:28:45
DLL version V5.00e, compiled Jun 23 2015 18:28:42
Firmware: J-Link ARM V7 compiled Jun 20 2012 19:45:53
Hardware: V7.00
S/N: 19087980
Feature(s): RDI, FlashDL, FlashBP, JFlash, GDBFull
VTarget = 3.377V
Info: TotalIRLen = 4, IRPrint = 0x01
Info: CP15.0.0: 0x41129200: ARM, Architecure 4T
Info: CP15.0.1: 0x0D172172: ICache: 16kB (64*8*32), DCache: 16kB (64*8*32)
Info: Cache type: Separate, Write-back, Format A
Found 1 JTAG device, Total IRLen = 4:
#0 Id: 0x0032409D, IRLen: 04, Unknown device
Found ARM with core Id 0x0032409D (ARM9)    // 识别出id则说明连接成功
Target interface speed: 100 kHz
J-Link>


4. 设置数据传输速度
J-Link>speed 12000  // 12kHz = 12MHz


5. 下载sdram初始化程序
J-Link>loadbin init_nand.bin 0      // 首次直接使用loadbin命令下载程序到sram的0地址
J-Link>setpc 0      // 设置pc寄存器
J-Link>g            // 跳转执行


6. 烧写u-boot.bin到sdram中
此时sdram已经通过init_nand.bin完成初始化，因此可以烧写到sdram中

J-Link>h            // 暂停cpu运行
J-Link>loadbin u-boot.bin 0x33f80000    // 下载uboot到链接地址位置
J-Link>setpc 0x33f80000     // 设置pc
J-Link>g            // 跳转到uboot执行程序


此时通过串口可以看到uboot输出信息
U-Boot 1.1.6-g81e0c984-dirty (Mar 11 2019 - 01:14:36)                          

DRAM:  64 MB                                                                   
Flash:  0 kB                                                                   
NAND:  256 MiB                                                                 
In:    serial                                                                  
Out:   serial                                                                  
Err:   serial                                                                  
Use these steps to program the image to flash:                                 
1. In OpenOCD                                                                  
   Run the 'halt' command to halt u-boot                                       
   Run the 'load_image <file> <address>' command to load file to SDRAM         
   Run the 'resume' command to resume u-boot                                   
2. In u-boot, use the flash commands to program the image to flash             
Or, use the tftp or nfs command to download file, and then program the flash.  
uboot>


7. 将要烧写到Nand的uboot程序下载到sdram起始地址
J-Link>h
J-Link>loadbin u-boot.bin 0x30000000
J-Link>g


8. 通过sdram上运行在0x33f80000地址的uboot来将0x30000000地址上的uboot烧写到Nand中
uboot> nand erase bootloader
uboot> nand write 0x30000000 bootloader


9. 以Nand方式重新上电启动，可以看到uboot输出信息
```

#### 2. 使用JLink间接烧写Nor Flash步骤

```
1. 以Nor方式对开发板进行上电

2. 进入J-Link命令行环境 (自动识别设备id)
$ JLinkExe

3. 设置数据传输速度
J-Link>speed 12000  // 12kHz = 12MHz


4. 下载sdram初始化程序
J-Link>loadbin init_nor.bin 0x40000000      // 首次直接使用loadbin命令下载程序到sram的0x40000000地址
J-Link>setpc 0x40000000      // 设置pc寄存器
J-Link>g            // 跳转执行，D10指示灯亮


5. 烧写u-boot.bin到sdram中
此时sdram已经通过init_nor.bin完成初始化，因此可以烧写到sdram中

J-Link>h            // 暂停cpu运行
J-Link>loadbin u-boot.bin 0x33f80000    // 下载uboot到链接地址位置
J-Link>setpc 0x33f80000     // 设置pc
J-Link>g            // 跳转到uboot执行程序，三个灯亮


此时通过串口可以看到uboot输出信息
U-Boot 1.1.6-g81e0c984-dirty (Mar 11 2019 - 01:14:36)

DRAM:  64 MB
Flash:  2 MB
NAND:  256 MiB
*** Warning - bad CRC or NAND, using default environment

In:    serial
Out:   serial
Err:   serial
Use these steps to program the image to flash:
1. In OpenOCD
   Run the 'halt' command to halt u-boot
   Run the 'load_image <file> <address>' command to load file to SDRAM
   Run the 'resume' command to resume u-boot
2. In u-boot, use the flash commands to program the image to flash
Or, use the tftp or nfs command to download file, and then program the flash.
uboot>


6. 将要烧写到Nand的uboot程序下载到sdram起始地址
J-Link>h
J-Link>loadbin u-boot.bin 0x30000000
J-Link>g


7. 通过sdram上运行在0x33f80000地址的uboot来将0x30000000地址上的uboot烧写到Nor中
注意：如果烧写的bin文件比擦除的扇区空间大，会导致无法启动uboot程序。
0x30000 = 3 x 2^16 = 3 x 2^6 k = 192 k
0x40000 = 4 x 2^16 = 4 x 2^6 k = 256 k

uboot> protect off all  // 解锁
Un-Protect Flash Bank # 1
uboot> erase 0 0x3ffff    // 擦除从0地址开始的大小为0x40000的NOR扇区，大小为擦除块的整数倍

...... done
Erased 7 sectors
uboot> cp.b 0x30000000 0 0x40000  // 把前面下载到0x30000000的程序烧写到Nor中
Copy to Flash... done


8. 以Nor方式重新上电启动，可以看到uboot输出信息
```


## 参考文章

- [Jlink 烧写文件到 nandflash norflash](https://blog.csdn.net/lizuobin2/article/details/52005917)


