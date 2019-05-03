#!/usr/bin/env bash

# usage:
# source envsetup-dnw.sh

# store the current script root path
Q_DNW_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)

DNW_ZIP_NAME="quokka-dnw-master"


OLDPWD_BAK=${OLDPWD}
# ===>enter
cd $Q_DNW_PATH_CUR

if [ ! -d $DNW_ZIP_NAME ]; then
    # install depend
    sudo apt-get install libusb-dev
    # extract source package
    unzip $DNW_ZIP_NAME.zip
    # build source and install
    cd $DNW_ZIP_NAME
    make
    sudo make install
fi

DNW_PATH="$Q_DNW_PATH_CUR/$DNW_ZIP_NAME"
export PATH=$DNW_PATH:$PATH


# <===exit
cd $Q_WORK_DIR_CUR
export OLDPWD=${OLDPWD_BAK}


# usage:
# dnw <file> <address>

