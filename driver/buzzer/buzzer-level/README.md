## 编译测试

```
编译内核模块及测试程序
$ make && make test && make clean

复制到NFS根文件系统
$ mkdir -p $NFS_ROOT_PATH/rootfs/test/buzzer/buzzer-level/
$ cp -fp *.ko *.out $NFS_ROOT_PATH/rootfs/test/buzzer/buzzer-level/

安装驱动模块
# insmod buzzer-level.ko

查看已安装模块信息
# lsmod
Module                  Size  Used by    Not tainted
buzzer_level            3364  0

卸载驱动模块
# rmmod buzzer_level

测试程序
# ./buzzer-level-test.out on                // 打开蜂鸣器
[debug] call buzzer_level_ioctl: mode = 1, duration = 0

# ./buzzer-level-test.out off               // 关闭蜂鸣器
[debug] call buzzer_level_ioctl: mode = 2, duration = 0

# ./buzzer-level-test.out ms 500            // 打开蜂鸣器，持续500ms后自动关闭
[debug] call buzzer_level_ioctl: mode = 3, duration = 500
[debug] call buzzer_level_ioctl: jiffies = 4294912753, HZ = 200
/test/buzzer/buzzer-level # [debug] call buzzer_timer_func: jiffies = 4294912854, HZ = 200
```


