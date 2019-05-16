## 移植格式化工具dosfstools

```
1. 下载dosfstools源码包
$ wget -O dosfstools-2.11.tar.gz https://codeload.github.com/dosfstools/dosfstools/tar.gz/v2.11


2. 解压并进入源码目录
$ tar -zxvf dosfstools-2.11.tar.gz
$ cd dosfstools-2.11


3. 编译生成mkdosfs等工具
$ make CC=arm-linux-gcc


4. 安装dosfstools到指定目录下
$ export DOSFSTOOLS_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/dosfstools/dosfstools-2.11
$ mkdir -p $DOSFSTOOLS_INSTALL_DIR
$ make PREFIX=$DOSFSTOOLS_INSTALL_DIR install


5. 查看安装的内容
$ ls $DOSFSTOOLS_INSTALL_DIR
sbin usr


6. 复制dosfstools编译生成的内容到开发板根文件系统中
$ cp -fdpr $DOSFSTOOLS_INSTALL_DIR/* $ROOTFS_ROOT_PATH/


7. 格式化分区及挂载
# mkdosfs /dev/ramblock1        // 格式化分区1
mkdosfs 2.11 (12 Mar 2005)
# mount /dev/ramblock1 tmp1     // 挂载分区1到tmp1目录
```


