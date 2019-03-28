## 目录说明

```
LED
D10 - GPF4
D11 - GPF5
D12 - GPF6


led-simple      // 简单LED驱动
led-minor       // 使用次设备号来标记每个LED
led-platform    // 平台设备与驱动
led-region      // 动态分配设备号
```


## 测试步骤

```
1. 主机上编译模块驱动和对应测试程序
$ make              // 编译内核模块
$ make test         // 编译测试程序
$ make clean        // 清理中间文件
$ make distclean    // 清理中间文件和目标文件

2. 复制到NFS网络文件系统的test目录下
$ mkdir -p $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/test/led/led-simple
$ cp -fp *.ko *.out $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/test/led/led-simple

3. 安装内核模块
# insmod led-simple.ko      // 安装内核模块
# lsmod                     // 查看已安装的内核模块 (模块名为led_simple，即横杠变成下划线)
# rmmod led_simple          // 删除指定内核模块 (模块名称需要用lsmod命令来查看)

4. 运行测试程序
# ./led-simple-test.out on
# ./led-simple-test.out off
```


