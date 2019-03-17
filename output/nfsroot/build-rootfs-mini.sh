#!/usr/bin/env bash

# create dev file: console null
# NOTE: it must be create in local environment
cd $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/
mkdir dev
cd dev
sudo mknod console c 5 1
sudo mknod null c 1 3
#ls -l
cd -

# create etc file: inittab
mkdir etc
cd etc
cat << EOF >> inittab
console::askfirst:-/bin/sh
EOF
cd -

# create lib directory
mkdir lib
cp -fpd $LINUX_ARM_ROOT_PATH/tools/arm-gcc/gcc-3.4.5-glibc-2.3.6/arm-linux/lib/*.so* lib/
#ls -l lib/


# create yaffs2 image
#cd $LINUX_ARM_ROOT_PATH/output/nfsroot/
# mkyaffs2image <dir> <image>
#mkyaffs2image rootfs rootfs.yaffs2

# create jffs2 image
#mkfs.jffs2 -n -s 2048 -e 128KiB -d rootfs -o rootfs.jffs2


