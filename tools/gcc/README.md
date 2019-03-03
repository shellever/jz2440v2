交叉编译工具链
--------------

```
下载交叉编译工具链 (4.4.3和4.3.2)
http://www.arm9.net/download.asp

// arm-linux-gcc-4.4.3
更新时间：2010-08-17
更新说明：符合EABI标准的交叉编译器，彻底解决编译Qtopia2/Qt4时出现段错误(Segmentation Fault)的问题
下载文件：arm-linux-gcc-4.4.3-20100728.tar.gz
配置方式：
$ tar -zxvf arm-linux-gcc-4.4.3-20100728.tar.gz
$ mkdir arm-linux-gcc-4.4.3
$ cp -frpd ./opt/FriendlyARM/toolchain/4.4.3/* arm-linux-gcc-4.4.3/
$ sudo rm -rf opt
$ export PATH=$HOME/linux-arm/tools/arm-gcc/arm-linux-gcc-4.4.3/bin:$PATH
$ arm-linux-gcc -v

// arm-linux-gcc-4.3.2
更新时间：2009-04-29
更新说明：早期版本的交叉编译器
下载文件：arm-linux-gcc-4.3.2.tgz
配置方式：
$ tar -zxvf arm-linux-gcc-4.3.2.tgz
$ mkdir arm-linux-gcc-4.3.2
$ mv usr/local/arm/4.3.2/* arm-linux-gcc-4.3.2/
$ rm -rf usr
$ export PATH=$HOME/linux-arm/tools/arm-gcc/arm-linux-gcc-4.3.2/bin:$PATH
$ arm-linux-gcc -v
```

