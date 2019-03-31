## 编译测试

```
编译内核模块及测试程序
$ make && make test

安装驱动模块
# insmod key-simple.ko

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
key_simple              2812  0

卸载驱动模块
# rmmod key_simple

测试程序
# ./key-simple-test              // 执行后，会打印按键信息
0000 key pressed: 1 1 0 1
......
0154 key pressed: 1 1 0 1

0155 key pressed: 1 0 1 1
......
0309 key pressed: 1 0 1 1

0310 key pressed: 0 1 1 1
......
0464 key pressed: 0 1 1 1
```

