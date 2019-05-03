#!/usr/bin/env bash

# usage:
# source envsetup-mkyaffs.sh

# store the current script root path
Q_MKYAFFS_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


OLDPWD_BAK=${OLDPWD}
# ===>enter
cd $Q_MKYAFFS_PATH_CUR

MKYAFFS_PATH="$Q_MKYAFFS_PATH_CUR"
export PATH=$MKYAFFS_PATH:$PATH

# <===exit
cd $Q_WORK_DIR_CUR
export OLDPWD=${OLDPWD_BAK}

