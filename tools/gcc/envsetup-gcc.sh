#!/usr/bin/env bash

# usage:
# source envsetup-gcc.sh

# store the current script root path
Q_GCC_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# GNU Arm Embedded Toolchain
# tar:  arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2
# path: gcc-3.4.5-glibc-2.3.6
ARM_LINUX_GCC_TAR="arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2"
ARM_LINUX_GCC_RAW_PATH="gcc-3.4.5-glibc-2.3.6"
ARM_LINUX_GCC="arm-linux-gcc-3.4.5"
TARFLAGS="-jxvf"


OLDPWD_BAK=${OLDPWD}
# ===>enter
cd $Q_GCC_PATH_CUR

if [ ! -d $ARM_LINUX_GCC ]; then
    tar $TARFLAGS $ARM_LINUX_GCC_TAR
    mv $ARM_LINUX_GCC_RAW_PATH $ARM_LINUX_GCC
fi

CROSS_TOOLCHAIN="$Q_GCC_PATH_CUR/$ARM_LINUX_GCC/bin"
export PATH=$CROSS_TOOLCHAIN:$PATH
export LINUX_ARM_TOOLS_GCC_PATH="$Q_GCC_PATH_CUR/$ARM_LINUX_GCC"

# <===exit
cd $Q_WORK_DIR_CUR
export OLDPWD=${OLDPWD_BAK}


# exported cmds
# arm-linux-gcc
# arm-linux-ld
# arm-linux-objcopy
# arm-linux-objdump

# testing
# arm-linux-gcc -v

