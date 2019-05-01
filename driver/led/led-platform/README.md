## 编译测试

```
编译内核模块及测试程序
$ make && make test

安装驱动模块
# insmod led-platform-dev.ko               // 先安装平台设备
# insmod led-platform-drv.ko               // 再安装平台驱动

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
led_platform_drv        4312  0
led_platform_dev        1888  0

卸载驱动模块
# rmmod led_platform_drv
# rmmod led_platform_dev

测试程序
# ./led-platform-test on      // 点亮LED D11
# ./led-platform-test off     // 熄灭LED D11
```
                                                                                

