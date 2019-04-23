## 编译测试

```
编译内核模块及测试程序
$ make && make test && make clean

安装驱动模块
# insmod key-mutex.ko

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
key_mutex               4648  0

卸载驱动模块
# rmmod key_mutex

测试程序
# ./key-mutex-test.out          // 执行后，会打印按键信息
key_val = 0x00, ret = -1
key_val = 0x81, ret = 1         // 按键按下
key_val = 0x81, ret = -1        // 互斥，非阻塞立即返回
key_val = 0x81, ret = -1
key_val = 0x81, ret = -1
key_val = 0x82, ret = 1
```

