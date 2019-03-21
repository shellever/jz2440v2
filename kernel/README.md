## kernel打补丁
```
# extract
$ tar -jxvf linux-2.6.22.6.tar.bz2

# patch
$ cd linux-2.6.22.6
$ patch -p1 < ../linux-2.6.22.6_jz2440.patch

# replace for 480x270 4.3inch lcd
$ mv ../mach-smdk2440_lcd_4.3in.c arch/arm/mach-s3c2440/mach-smdk2440.c 

# re-package
$ cd ..
$ tar -cjf linux-2.6.22.6_jz2440.tar.bz2 linux-2.6.22.6
```


## kernel编译
```
# specify the output directory
#mkdir -p $KERNEL_ROOT_PATH/build
#export KBUILD_OUTPUT=$KERNEL_ROOT_PATH/build
cd $KERNEL_ROOT_PATH

# make distclean

# config
#cp config_ok .config
cp -p config_ok arch/arm/configs/jz2440v2_defconfig
make jz2440v2_defconfig    // 生成默认配置 .config
make menuconfig            // 图形界面修改默认配置
# build
make uImage

# output
# arch/arm/boot/uImage
```


