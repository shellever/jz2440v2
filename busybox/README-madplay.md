## 移植madplay

```
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


