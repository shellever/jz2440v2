## 使用异步通知机制来获取按键值

```
编译内核模块及测试程序
$ make && make test

安装驱动模块
# insmod key-fasync.ko

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
key_fasync              4392  0 

卸载驱动模块
# rmmod key_fasync

测试程序
# ./key-fasync-test.out     // 执行后，会打印按键信息
key_val = 0x03              // 按下
key_val = 0x83              // 松开
key_val = 0x02
key_val = 0x82
key_val = 0x01
key_val = 0x81
key_val = 0x01
key_val = 0x81
key_val = 0x04
key_val = 0x84
```


