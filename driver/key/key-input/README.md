## 编译测试

```
编译内核模块及测试程序
$ make && make clean

安装驱动模块
# insmod key-input.ko
input: Unspecified device as /class/input/input1

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
key_input               2656  0

卸载驱动模块
# rmmod key_input

测试程序 (生成/dev/event1设备文件)
# ls -l /dev/event*
crw-rw----    1 0        0         13,  64 Jan  1 00:00 /dev/event0
crw-rw----    1 0        0         13,  65 Jan  1 00:08 /dev/event1

没有启动QT时，执行下面命令，然后依次按 S2 S3 S4 (l s enter)按键，就可以在LCD上显示 ls 字符串
# cat /dev/tty1
```
                                                                                
