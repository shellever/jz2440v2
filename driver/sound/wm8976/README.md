## 编译测试

```
//
// 简单测试wm8976驱动
//
1. 确认内核配置开启I2S音频数据传输接口
$ cp -rp linux-2.6.22.6 linux-2.6.22.6-sound
$ cd linux-2.6.22.6-sound
$ make menuconfig
Device Drivers  --->
    Sound  --->
        Advanced Linux Sound Architecture  --->
            System on Chip audio support  --->
                <*> I2S of the Samsung S3C24XX chips

2. 修改Makefile，去掉默认的声卡驱动
$ vi sound/soc/s3c24xx/Makefile
//obj-y += s3c2410-uda1341.o

3. 重新编译内核
$ make uImage
$ mkdir -p $ROOTFS_ROOT_PATH/test/sound/wm8976
$ cp -fp arch/arm/boot/uImage $ROOTFS_ROOT_PATH/test/sound/wm8976/

4. 编译声卡驱动程序
$ vi Makefile
KERN_DIR ?= $(KERNEL_ROOT_PATH)/../linux-2.6.22.6-sound
$ make distclean && make && make clean
$ cp -fp wm8976.ko $ROOTFS_ROOT_PATH/test/sound/wm8976/

4. 使用新内核启动
重启系统进入uboot命令行界面，使用nfs命令下载uImage镜像，使用bootm命令进入系统
> nfs 30000000 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot/rootfs/test/sound/wm8976/uImage
> bootm 30000000

5. 安装wm8976驱动
# cd test/sound/wm8976
# insmod wm8976.ko

6. 查看生成的设备文件
# ls -l /dev/dsp /dev/mixer
crw-rw----    1 0        0         14,   3 Jan  1 00:00 /dev/dsp
crw-rw----    1 0        0         14,   0 Jan  1 00:00 /dev/mixer

7. 播放wav文件 (需要插上耳机)
# cat PSI-missing.wav > /dev/dsp    // 魔法禁书目录

8. 录音测试
8.1 输入命令后对着麦克风说话，按 ctrl+c 退出录音
# cat /dev/dsp > sound.bin
8.2 播放录音
# cat sound.bin > /dev/dsp



//
// 使用madplay播放mp3文件
//
0. 编译 zlib-1.2.3 库
$ tar -zxvf zlib-1.2.3.tar.gz
$ cd zlib-1.2.3
$ export ZLIB_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/zlib/zlib-1.2.3
$ mkdir -p $ZLIB_INSTALL_DIR 
$ CC=arm-linux-gcc ./configure --host=arm-linux --prefix=$ZLIB_INSTALL_DIR
$ make
$ make install

1. 编译 libid3tag-0.15.1b 库
$ tar -zxvf libid3tag-0.15.1b.tar.gz
$ cd libid3tag-0.15.1b
$ export LIBID3TAG_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/libid3tag/libid3tag-0.15.1b
$ mkdir -p $LIBID3TAG_INSTALL_DIR
$ ./configure --host=arm-linux --prefix=$LIBID3TAG_INSTALL_DIR LDFLAGS="-L$MADPLAY_INSTALL_DIR/lib" CFLAGS="-I$MADPLAY_INSTALL_DIR/include"
$ make
$ make install

2. 编译 libmad-0.15.1b 库
$ tar -zxvf libmad-0.15.1b.tar.gz
$ cd libmad-0.15.1b
$ export LIBMAD_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/libmad/libmad-0.15.1b
$ mkdir -p $LIBMAD_INSTALL_DIR
$ ./configure --host=arm-linux --prefix=$LIBMAD_INSTALL_DIR
$ make
$ make install

3. 编译 madplay 应用
$ tar -zxvf madplay-0.15.2b.tar.gz
$ cd madplay-0.15.2b/
$ export MADPLAY_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/madplay/madplay-0.15.2b
$ mkdir -p $MADPLAY_INSTALL_DIR
$ cp -fpdr $ZLIB_INSTALL_DIR/* $MADPLAY_INSTALL_DIR/            // zlib
$ cp -fpdr $LIBID3TAG_INSTALL_DIR/* $MADPLAY_INSTALL_DIR/       // libid3tag
$ cp -fpdr $LIBMAD_INSTALL_DIR/* $MADPLAY_INSTALL_DIR/          // libmad
$ ./configure --host=arm-linux --prefix=$MADPLAY_INSTALL_DIR LDFLAGS="-L$MADPLAY_INSTALL_DIR/lib" CFLAGS="-I$MADPLAY_INSTALL_DIR/include"
$ make
$ make install

4. 复制madplay到根文件系统
$ cp -fpdr $MADPLAY_INSTALL_DIR/* $ROOTFS_ROOT_PATH/

6. 使用madplay工具播放mp3文件
按小键盘的减号"-"降低音量，按小键盘的加号"+"提高音量
# madplay --tty-control Whiteout.mp3
```


