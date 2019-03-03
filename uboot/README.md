## build u-boot

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


## patch u-boot

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
