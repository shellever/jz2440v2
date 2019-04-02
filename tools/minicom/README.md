## 添加minicom使用USB转串口工具权限

```
1. 使用 lsusb 命令查看 usb 设备的厂商和产品id
idVendor:idProduct = 067b:2303
$ lsusb
Bus 001 Device 004: ID 067b:2303 Prolific Technology, Inc. PL2303 Serial Port
Bus 001 Device 005: ID 5345:1234 Owon PDS6062T Oscilloscope

2. 查看当前设备文件的权限
其他用户没有写权限，导致在使用minicom时需要sudo来授权读写此设备文件
$ ls -l /dev/bus/usb/001/004
crw-rw-r-- 1 root root 189, 3 Apr  2 21:07 /dev/bus/usb/001/004   

3. 创建串口转USB设备文件的udev规则
需要用到lsusb命令查询得到的串口转USB设备的厂商id和产品id，
然后主要时需要将权限修改为666即可
$ sudo vi /etc/udev/rules.d/50-usb-serial.rules
SUBSYSTEMS=="usb", ATTRS{idVendor}=="067b", ATTRS{idProduct}=="2303", MODE="0666"

4. 重启udev服务
$ sudo service udev restart

5. 重新拔插串口转USB工具

6. 查看修改后的设备权限
可以知道权限已经修改成666，即可直接使用minicom来运行，而不需要sudo来授权
$ lsusb
Bus 001 Device 007: ID 067b:2303 Prolific Technology, Inc. PL2303 Serial Port
Bus 001 Device 006: ID 5345:1234 Owon PDS6062T Oscilloscope
$ ls -l /dev/bus/usb/001/007
crw-rw-rw- 1 root root 189, 6 Apr  3 00:58 /dev/bus/usb/001/007

7. 关闭原先minicom终端，重新开启终端执行minicom进行测试 (well done!)
$ minicom
```


