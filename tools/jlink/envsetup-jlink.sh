#!/usr/bin/env bash

# usage:
# source envsetup-jlink.sh

# store the current script root path
Q_JLINK_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


JLINK_V635G="JLink_Linux_V635g_x86_64"
JLINK_V500E="JLink_Linux_V500e_x86_64"
JLINK_V=$JLINK_V500E

# ===>enter
cd $Q_JLINK_PATH_CUR

if [ ! -d $JLINK_V ]; then
    tar -zxvf $JLINK_V.tgz
fi

JLINK_PATH="$Q_JLINK_PATH_CUR/$JLINK_V"
export PATH=$JLINK_PATH:$PATH

if [ ! -f /etc/udev/rules.d/99-jlink.rules ]; then
    sudo cp -fp $JLINK_PATH/99-jlink.rules /etc/udev/rules.d/
    sudo service udev restart
fi

# <===exit
cd $Q_WORK_DIR_CUR

