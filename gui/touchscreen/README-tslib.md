## 编译安装tslib

```
1. 安装依赖包
$ sudo apt-get install -y autoconf
$ sudo apt-get install -y automake
$ sudo apt-get install -y libtool


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
$ make


6. 安装到指定目录下
$ make install


7. 查看tslib安装内容
$ ls $TSLIB_INSTALL_DIR
bin etc include lib
```


## 配置使用tslib

```
方式一：修改根文件系统里面配置 (开发板配置)
1. 修改 rootfs/etc/ts.conf 第1行，去掉#号和第一个空格
// 原来配置
# module_raw input
// 修改后
module_raw input

2. 添加环境变量
$ vi rootfs/etc/profile     // pc上修改
export TSLIB_TSDEVICE=/dev/event0           # touchscreen 触摸屏设备文件
export TSLIB_CALIBFILE=/etc/pointercal      # 校准数据文件，由 ts_calibrate 校准程序生成
export TSLIB_CONFFILE=/etc/ts.conf          # tslib 配置文件
export TSLIB_PLUGINDIR=/lib/ts              # tslib 插件目录
export TSLIB_CONSOLEDEVICE=none             # 控制台设备文件，默认为/dev/tty
export TSLIB_FBDEVICE=/dev/fb0              # framebuffer 设备文件

# source /etc/profile       // 开发板上执行生效


方式二：复制整个tslib编译生成的安装目录，再进行配置
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
```


