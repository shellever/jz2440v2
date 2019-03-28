## 目录说明

```
led         - LED灯驱动
key         - 按键驱动
buzzer      - 蜂鸣器驱动
flash       - Flash驱动
lcd         - 显示屏驱动
ts          - 触摸屏驱动
net         - 网卡驱动
dma         - DMA驱动
sound       - 声卡驱动
uart        - 串口驱动
usb         - USB驱动
i2c         - I2C驱动
spi         - SPI驱动

Makefile
```


## 编译方式

```
1. 配置编译环境
$ source build/envsetup.sh

2. 编译指定内核模块 (*.ko) 和测试程序 (*.out)
$ cd driver/led/led-simple
$ make          // *.ko
$ make test     // *.out

3. 编译全部内核模块和测试程序
$ cd driver
$ make
$ make test

4. 归档所有*.ko和*.out文件到output/driver目录下
$ make archive
```


