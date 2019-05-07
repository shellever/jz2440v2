## 编译测试

```
去掉内核默认的触摸屏驱动
$ make menuconfig
Device Drivers  --->
    Input device support  --->
        Touchscreens  --->
            <> S3C2410/S3C2440 touchscreens

重新编译内核镜像文件uImage及lcd驱动依赖的相关模块
$ make uImage
$ make modules
$ mkdir -p $NFS_ROOT_PATH/rootfs/test/ts/
$ cp -fp arch/arm/boot/uImage $NFS_ROOT_PATH/rootfs/test/ts/
$ cp -fp drivers/video/cfbcopyarea.ko $NFS_ROOT_PATH/rootfs/test/ts/
$ cp -fp drivers/video/cfbfillrect.ko $NFS_ROOT_PATH/rootfs/test/ts/
$ cp -fp drivers/video/cfbimgblt.ko $NFS_ROOT_PATH/rootfs/test/ts/

编译触摸屏驱动程序
$ make && make clean
$ cp -fp ts.ko $NFS_ROOT_PATH/rootfs/test/ts/

编译显示屏驱动程序
$ make && make clean
$ cp -fp lcd.ko $NFS_ROOT_PATH/rootfs/test/ts/


重启系统进入uboot命令行界面，使用nfs命令下载uImage镜像，使用bootm命令进入系统
> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/ts/uImage
> bootm    // 或者bootm 30000000


安装ts驱动程序
# insmod ts.ko

查看安装模块信息
/test/ts # lsmod
Module                  Size  Used by    Not tainted
ts                      1992  0

使用触摸笔按下或松开可打印如下信息：
/test/ts # pen down
pen up
pen down        // 按下
pen up          // 松开



//
// 编译tslib源码
//
1. 安装依赖工具
$ sudo apt-get install -y autoconf automake libtool

2. 解压tslib源码包，并重命名目录
$ tar -zxvf tslib-1.4.tar.gz
$ mv tslib tslib-1.4
$ cd tslib-1.4

3. 自动配置编译tslib所需的环境变量
$ ./autogen.sh

4. 配置生成Makefile
$ export TSLIB_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/gui/tslib/tslib-1.4
$ mkdir -p $TSLIB_INSTALL_DIR
// 产生缓冲文件和配置
// 直接运行 configure 脚本相关的代码，会出现交叉编译错误: undefined reference to `rpl_malloc'，
// 这是由 ac_cv_func_malloc_0_nonnull 检查引起的，为了不让它检查，
// 产生一个 cache 文件 arm-linux.cache，欺骗 configure 再执行
$ echo "ac_cv_func_malloc_0_nonnull=yes" > arm-linux.cache

// --host=<arm-linux>           指定编译工具前缀
// --cache-file=<cache-file>    指定缓冲文件
// --prefix=<install-dir>       指定安装目录 (需要绝对路径，否则会错误)
$ ./configure --host=arm-linux --cache-file=arm-linux.cache --prefix=/home/linuxfor/workspace/jz2440v2/output/gui/tslib/tslib-1.4

5. 编译tslib源码
//$ make 2>&1 | tee build-tslib.log
$ make

6. 安装到指定目录下
$ make install


7. 查看编译安装内容
$ ls $TSLIB_INSTALL_DIR
bin etc include lib



//
// 配置tslib
//
方式一：修改根文件系统里面配置 (开发板配置)
1. 修改 rootfs/etc/ts.conf 第1行，去掉#号和第一个空格
// 原来配置
# module_raw input
// 修改后
module_raw input

2. 添加环境变量
$ vi rootfs/etc/profile    // pc上修改
export TSLIB_TSDEVICE=/dev/event0          # touchscreen 触摸屏设备文件
export TSLIB_CALIBFILE=/etc/pointercal      # 校准数据文件，由 ts_calibrate 校准程序生成
export TSLIB_CONFFILE=/etc/ts.conf          # tslib 配置文件
export TSLIB_PLUGINDIR=/lib/ts              # tslib 插件目录
export TSLIB_CONSOLEDEVICE=none            # 控制台设备文件，默认为/dev/tty
export TSLIB_FBDEVICE=/dev/fb0              # framebuffer 设备文件

# source /etc/profile      // 开发板上执行生效


方式二：复制整个tslib编译生成的安装目录，再进行配置 (v)
1. 复制tslib安装生成的整个目录内容到指定路径下 (主机上)
$ cp -prfd $TSLIB_INSTALL_DIR $NFS_ROOT_PATH/rootfs/test/ts/

2. 修改ts.conf配置文件 (开发板上)
# vi tslib/etc/ts.conf
module_raw input            // 将此行去掉注释符号和空格，使之生效

3. 配置环境变量 (开发板上)
export TSLIB_ROOT=/test/ts/tslib-1.4    // 配置此tslib根路径环境变量，方便测试
export TSLIB_TSDEVICE=/dev/event0
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CALIBFILE=$TSLIB_ROOT/etc/pointercal
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TSLIB_ROOT/lib



安装lcd驱动程序 (需要在编译ts驱动时的内核上重新编译)
# insmod cfbcopyarea.ko
# insmod cfbfillrect.ko
# insmod cfbimgblt.ko
# insmod lcd.ko 
安装ts驱动程序
# insmod ts.ko

使用脚本自动安装ko文件
/test/ts # ./test-ts.sh
Console: switching to colour frame buffer device 60x34
input: touchscreen device as /class/input/input0


查看lcd和ts设备文件
/test/ts # ls -l /dev/event0
crw-rw----    1 0        0         13,  64 Jan  1 00:01 /dev/event0
/test/ts # ls -l /dev/fb0
crw-rw----    1 0        0         29,   0 Jan  1 00:00 /dev/fb0


//
// 运行tslib库中的测试程序
//
1. 电阻屏触摸自动校准 (首次使用必须先校准)
按照十字光标位置依次按下 (左上 -> 右上 -> 右下 -> 左下 -> 中间)
/test/ts/tslib/bin # ./ts_calibrate
xres = 480, yres = 272
Took 26 samples...
Top left : X =  296 Y =  165
Took 15 samples...
Top right : X =  316 Y =  862
Took 14 samples...
Bot right : X =  776 Y =  863
Took 13 samples...
Bot left : X =  776 Y =  166
Took 12 samples...
Center : X =  547 Y =  513
-39.505920 -0.001147 0.545209
-59.617920 0.365755 -0.005244
Calibration constants: -2589060 -75 35730 -3907120 23970 -343 65536



2. 十字光标点拖动及绘画测试
/test/ts/tslib/bin # ./ts_test
673.902667:    241    140      1
673.920593:    238    140      1
673.940624:    238    141      1
673.960600:    238    141      1
674.000590:    237    142      1
674.020603:    237    142      1



3. 打印转换过的坐标数值
/test/ts/tslib/bin # ./ts_print
806.863155:    408     83      1
806.880589:    408     83      1
806.900612:    408     83      1
806.920592:    408     83      1
806.940599:    408     83      1
806.960592:    408     83      1
806.980595:    408     83      1



4. 打印原始电压数值
/test/ts/tslib/bin # ./ts_print_raw
828.301407:    554    484      1
828.320618:    553    482      1
828.340595:    553    483      1
828.360597:    560    488      1
828.380598:    569    495      1
828.400597:    577    503      1
828.420597:    585    505      1
828.440598:    589    512      1



5. 触摸笔按压力度测试
/test/ts/tslib/bin # ./ts_harvest
Took 5 samples...
Took 6 samples...
Took 8 samples...
Took 30 samples...
```


