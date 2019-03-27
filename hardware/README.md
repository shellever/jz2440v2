# 裸板程序

## 目录说明

```
adc_ts      -- ADC触摸屏使用
gpio/       -- gpio使用
  key_led   -- 按键控制LED
  led_on_s  -- 汇编语言点亮LED1
  led_on_c  -- C语言点亮LED1
  leds      -- 轮流点亮3个LED
i2c         -- i2c使用
init        -- 下载uboot前对sdram进行初始化
int         -- 中断使用
lcd         -- 适配4.3寸(480x272)和3.5寸(240x320)LCD使用
mmu         -- MMU使用
nand        -- nand存储器使用
sdram       -- sdram使用
stdio       -- 标准输入输出
timer       -- 定时器使用
uart        -- 串口使用


hardware.tar.bz2    -- 原始裸板程序的备份压缩文件
```


## 编译方式

```
1. 配置编译环境
$ source build/envsetup.sh

2. 编译指定裸板程序
$ cd hardware/gpio/led_on_c
$ make

3. 编译全部裸板程序
$ cd hardware
$ make

4. 归档所有bin文件到output目录下
$ make archive
```


## 烧写方式

```
通过NOR Flash上的uboot程序来烧写裸板程序到NAND Flash中

1. 使用dnw工具烧写裸板程序
1.1 先以NOR模式启动，进入uboot菜单选项界面
1.2 选择 n 选项，即使用dnw工具通过usb方式烧写bin文件到NAND中
1.3 在主机上执行命令 dnw <binfile> 来传送相应bin文件
    $ dnw led_on_s.bin
1.4 开发板下电，切换至NAND模式启动，即可测试裸板程序效果

2. 使用tftp命令烧写裸板程序
2.1 先以NOR模式启动，进入uboot菜单选项界面
2.2 选择 q 选项，退出菜单界面到命令行界面
2.3 使用tftp命令通过网络方式来下载裸板程序到SDRAM中，然后通过nand命令擦除写入到NAND中
    > tftp 30000000 led_on_s.bin
    > nand erase bootloader
    > nand write 30000000 bootloader
2.4 开发板下电，切换至NAND模式启动，即可测试裸板程序效果
```


