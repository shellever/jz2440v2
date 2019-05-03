## 编译测试

```
// usb驱动测试 (简单测试)
1. 修改内核编译选项，去掉原来的usb鼠标驱动
$ make menuconfig
Device Drivers  --->
    HID Devices  --->
        < > USB Human Interface Device (full HID) support


2. 编译uImage使用新的内核启动
$ make uImage
$ cp -fp arch/arm/boot/uImage $NFS_ROOT_PATH/rootfs/test/usb/

> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/usb/uImage
> bootm             // 或者 bootm 30000000


3. 安装usb驱动模块
# insmod usb-mouse-key.ko
enter usb_mouse_key_probe
input: mouse-keys as /class/input/input1
usbcore: registered new interface driver usb_mouse_key

# rmmod usb_mouse_key
usbcore: deregistering interface driver usb_mouse_key
enter usb_mouse_key_disconnect

# lsmod
Module                  Size  Used by    Not tainted
usb_mouse_key           2240  0 


4. 拔插usb鼠标设备，查看打印信息
# usb 1-1: new full speed USB device using s3c2410-ohci and address 4   // 插入鼠标
usb 1-1: configuration #1 chosen from 1 choice
bcdUSB    = 200
idVendor  = 0x24AE
idProduct = 0x1100
usb 1-1: USB disconnect, address 4        // 拔出鼠标



// usb驱动测试 (urb请求块提交测试)
查看鼠标设备信息
# ls -l /dev/event*

操作鼠标并观察打印数据
// 左键按下松开
data cnt 467: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
data cnt 468: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
// 右键按下松开
data cnt 473: 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
data cnt 474: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
// 中键按下松开
data cnt 475: 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
data cnt 476: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

8  4  2  1
0  1  1  1
-  M  R  L

USB鼠标数据含义
data[0]: 
    bit0-左键, 1-按下, 0-松开
    bit1-右键, 1-按下, 0-松开
    bit2-中键, 1-按下, 0-松开



查看usb鼠标按键设备文件
# ls -l /dev/event*
crw-rw----    1 0        0         13,  64 Jan  1 02:45 /dev/event0

使用tty1进行测试，依次按下鼠标左键，右键，最后按中键即可显示出来
# cat /dev/tty1
ls
lll


或者通过hexdump命令来查看
# hexdump /dev/event0      // 最后一个eventx，并不一定是event0

// Left
# hexdump /dev/event0
0000000 288b 0000 f597 0003 0001 0026 0001 0000
0000010 288b 0000 f5a2 0003 0000 0000 0000 0000
0000020 288b 0000 8bd2 0005 0001 0026 0000 0000
0000030 288b 0000 8bda 0005 0000 0000 0000 0000
// Right
# hexdump /dev/event0
0000000 289d 0000 4280 0007 0001 001f 0001 0000
0000010 289d 0000 428b 0007 0000 0000 0000 0000
0000020 289d 0000 55b6 0009 0001 001f 0000 0000
0000030 289d 0000 55bd 0009 0000 0000 0000 0000

// Middle
# hexdump /dev/event0
0000000 28b1 0000 7642 0009 0001 001c 0001 0000
0000010 28b1 0000 764c 0009 0000 0000 0000 0000
0000020 28b1 0000 4af8 000b 0001 001c 0000 0000
0000030 28b1 0000 4b00 000b 0000 0000 0000 0000

0000000 28b1 0000 7642 0009 0001 001c 0001 0000
28b1 0000 - 秒 (事件发生的时间)
7642 0009 - 微秒 (事件发生的时间)
0001 - 按键类事件 (EV_KEY - 0x01)
001c - 具体按键为回车键 (KEY_ENTER - 28) -> 0x1c
0001 0000 - 按键按下
```


