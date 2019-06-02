## 硬件说明

```
i2c存储芯片
AT24C02

设备地址
0xA0

i2c驱动使用高7位作为地址，即为0xA0>>1=0x50

S3C2440的i2c引脚配置 (由于没有引出来，直接在摄像头引脚上接即可)
IICSCL  - GPE14
IICSDA  - GPE15

VCC - Pin17
GND - Pin20
SCL - Pin2
SDA - Pin1
```


## 编译测试

```
编译内核模块及测试程序
$ make && make test && make clean

安装驱动模块
# insmod at24cxx.ko
call at24cxx_detect         // 有回调此函数，说明已经发现i2c设备

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
at24cxx                 2368  0

卸载驱动模块
# rmmod at24cxx

测试程序
# ./at24cxx-test.out                                          
rbuf:                                                                           
 20 19 06 01 18 10 A2                                                           
wbuf:                                                                           
 20 19 06 01 18 10 5F                                                           
rbuf:                                                                           
 20 19 06 01 18 10 5F
```


