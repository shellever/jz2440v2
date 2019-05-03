#!/usr/bin/env bash

# usage:
# source envsetup-minicom.sh

# store the current script root path
Q_MINICOM_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# minicom configuration
MINIRC_DEFAULT="minirc.dfl"
MINIRC_TTYUSB0="minirc.ttyusb0"


OLDPWD_BAK=${OLDPWD}
# ===>enter
cd $Q_MINICOM_PATH_CUR


if [ -z "$(which minicom)" ]; then
    sudo apt-get install -y minicom
    sudo apt-get install -y lrzsz
    sudo cp -fp 50-usb-serial.rules /etc/udev/rules.d/
fi

if [ ! -f "/etc/minicom/$MINIRC_TTYUSB0" ]; then
    sudo cp -fp $MINIRC_TTYUSB0 /etc/minicom/$MINIRC_TTYUSB0
fi

if [ ! -f "/etc/minicom/$MINIRC_DEFAULT" ]; then
    sudo cp -fp $MINIRC_DEFAULT /etc/minicom/$MINIRC_DEFAULT
fi

# config MINICOM environment variable
# -w        // turn line-wrap on at startup by default 
# -c on     // turn color support on
#export MINICOM="-w -c on"

alias sminicom='minicom -w -c on ttyusb0'
alias minicom='minicom -w'


# <===exit
cd $Q_WORK_DIR_CUR
export OLDPWD=${OLDPWD_BAK}


# configs interface
# sudo minicom -s

# exported cmds
# minicom
# sminicom

# startup
# minicom -w ttyusb0
# minicom -w -c on ttyusb0


