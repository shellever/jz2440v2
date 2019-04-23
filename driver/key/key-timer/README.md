## 编译测试

```
编译内核模块及测试程序
$ make && make test && make clean

安装驱动模块
# insmod key-timer.ko

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
key_timer               5028  0

卸载驱动模块
# rmmod key_timer

测试程序
# ./key-timer-test.out      // 执行后，会打印按键信息
enter key_timer_open
key_val = 0x01, ret = 1     // 按下
key_val = 0x81, ret = 1     // 松开
key_val = 0x02, ret = 1
key_val = 0x82, ret = 1
```

