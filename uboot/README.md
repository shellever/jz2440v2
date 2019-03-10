## u-boot编译

```
# specify the output directory
#export BUILD_DIR=$UBOOT_ROOT_PATH/build
cd $UBOOT_ROOT_PATH
make distclean
make 100ask24x0_config
make

# output
# tools/mkimage
```


## u-boot打补丁

```
# extract
tar -jxvf u-boot-1.1.6.tar.bz2

# patch
cd u-boot-1.1.6
patch -p1 < ../uboot-1.1.6_jz2440.patch

# re-package
cd ..
tar -jcvf u-boot-1.1.6_jz2440.tar.bz2 u-boot-1.1.6
```


## u-boot命令

命令格式：
```
U_BOOT_CMD(name,maxargs,rep,cmd,usage,help)

```
参数说明：
```
|参数|含义|
|----|----|
|name|命令的名称，不需要用引号括起来|
|maxargs|命令的最大参数个数，至少为1，表示命令本身|
|rep|是否自动重复，为1则下次直接按回车键会重复执行此命令|
|cmd|命令对应的处理函数，直接使用函数名|
|usage|简短的使用说明，在直接执行help命令时显示|
|help|使用help来查看指定命令的帮助信息时显示|
```


