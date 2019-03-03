#!/usr/bin/env bash

# usage:
# source envsetup-minicom.sh

# store the current script root path
Q_MINICOM_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# minicom configuration
MINIRC_DEFAULT="minirc.dfl"
MINIRC_JZ2440="minirc.ttyusb0"


# ===>enter
cd $Q_MINICOM_PATH_CUR


if [ -z "$(which minicom)" ]; then
    sudo apt-get install minicom
fi

if [ ! -f "/etc/minicom/$MINIRC_JZ2440" ]; then
    sudo cp -fp $MINIRC_JZ2440 /etc/minicom/$MINIRC_JZ2440
fi

# config MINICOM environment variable
# -w        // turn line-wrap on at startup by default 
# -c on     // turn color support on
#export MINICOM="-w -c on"

alias sminicom='sudo minicom -w -c on ttyusb0'


# <===exit
cd $Q_WORK_DIR_CUR


# configs interface
# sudo minicom -s

# exported cmds
# minicom
# sminicom

# startup
# sudo minicom -w ttyusb0
# sudo minicom -w -c on ttyusb0


