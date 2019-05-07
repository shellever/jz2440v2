#!/usr/bin/env bash

# usage:
# source envsetup.sh

# store the current script root path
PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
LINUX_ARM_ROOT_PATH=$(cd $PATH_CUR/..; pwd)

export LINUX_ARM_ROOT_PATH

# export variables
source $LINUX_ARM_ROOT_PATH/uboot/envsetup-uboot.sh
source $LINUX_ARM_ROOT_PATH/kernel/envsetup-kernel.sh
source $LINUX_ARM_ROOT_PATH/busybox/envsetup-busybox.sh

# export tools
source $LINUX_ARM_ROOT_PATH/tools/gcc/envsetup-gcc.sh
source $LINUX_ARM_ROOT_PATH/tools/dnw/envsetup-dnw.sh
source $LINUX_ARM_ROOT_PATH/tools/mkimage/envsetup-uboot.sh
source $LINUX_ARM_ROOT_PATH/tools/mkyaffs/envsetup-mkyaffs.sh
source $LINUX_ARM_ROOT_PATH/tools/mkjffs/envsetup-mkjffs.sh
source $LINUX_ARM_ROOT_PATH/tools/jlink/envsetup-jlink.sh

# network
source $LINUX_ARM_ROOT_PATH/tools/nfs/envsetup-nfs.sh
source $LINUX_ARM_ROOT_PATH/tools/tftp/envsetup-tftp.sh

# serial port
source $LINUX_ARM_ROOT_PATH/tools/minicom/envsetup-minicom.sh


# print the exported variables
echo
echo "the current exported variables for jz2440v2:"
echo "LINUX_ARM_ROOT_PATH  =${LINUX_ARM_ROOT_PATH}"
echo "UBOOT_ROOT_PATH      =${UBOOT_ROOT_PATH}"
echo "KERNEL_ROOT_PATH     =${KERNEL_ROOT_PATH}"
echo "BUSYBOX_ROOT_PATH    =${BUSYBOX_ROOT_PATH}"
echo "ARM_LINUX_GCC_PATH   =${ARM_LINUX_GCC_PATH}"
echo "NFS_ROOT_PATH        =${NFS_ROOT_PATH}"
echo "TFTP_ROOT_PATH       =${TFTP_ROOT_PATH}"
echo


