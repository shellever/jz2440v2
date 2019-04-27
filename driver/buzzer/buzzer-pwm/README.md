## 编译测试 (尚未验证)

```
编译内核模块及测试程序
$ make && make test && make clean

复制到NFS根文件系统
$ mkdir -p $NFS_ROOT_PATH/rootfs/test/buzzer/buzzer-pwm/
$ cp -fp *.ko *.out $NFS_ROOT_PATH/rootfs/test/buzzer/buzzer-pwm/

安装驱动模块
# insmod buzzer-pwm.ko

查看已安装模块信息

卸载驱动模块
# rmmod buzzer_pwm

测试程序
# ./buzzer-pwm-test.out
```


## 参考文章

[MINI2440开发板PWM驱动与测试程序简要分析](https://blog.csdn.net/ssdsafsdsd/article/details/8517602)

