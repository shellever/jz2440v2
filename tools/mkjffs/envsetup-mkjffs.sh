#!/usr/bin/env bash

# usage:
# source envsetup-mkjffs.sh

# store the current script root path
Q_MKJFFS_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# ===>enter
cd $Q_MKJFFS_PATH_CUR

MKJFFS_PATH="$Q_MKJFFS_PATH_CUR"
export PATH=$MKJFFS_PATH:$PATH 

# Memory Technology Device Utilities
[ -z "`which mkfs.jffs2`" ] && {
    sudo apt-get install mtd-utils
}

# <===exit
cd $Q_WORK_DIR_CUR

