#!/usr/bin/env bash

# usage:
# source envsetup-uboot.sh

# store the current script root path
Q_UBOOT_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


OLDPWD_BAK=${OLDPWD}
# ===>enter
cd $Q_UBOOT_PATH_CUR

UBOOT_TOOLS_PATH="$Q_UBOOT_PATH_CUR/u-boot-1.1.6"
export PATH=$UBOOT_TOOLS_PATH:$PATH

# other way
# sudo apt-get install u-boot-tools

# <===exit
cd $Q_WORK_DIR_CUR
export OLDPWD=${OLDPWD_BAK}

# exported cmds
# mkimage

