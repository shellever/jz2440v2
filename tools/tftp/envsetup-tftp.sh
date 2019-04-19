#!/usr/bin/env bash

# usage:
# source envsetup-tftp.sh

# store the current script root path
Q_TFTP_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# exported directory of tftp
TFTP_ROOT_PATH="$LINUX_ARM_ROOT_PATH/output/tftpboot"
if [ ! -e $TFTP_ROOT_PATH ]; then
    mkdir -p $TFTP_ROOT_PATH
fi
export TFTP_ROOT_PATH


if [ ! -e /etc/default/tftpd-hpa ]; then
# installation {{{
sudo apt-get install -y tftp-hpa tftpd-hpa
# configure tftp directory
KEY_NAME=TFTP_DIRECTORY
KEY_VALUE=$TFTP_ROOT_PATH
CONF_FILE=/etc/default/tftpd-hpa
LINE_NUM=`grep -n "${KEY_NAME}=" $CONF_FILE | awk -F: '{print $1}'`
sudo sed -i "${LINE_NUM}c ${KEY_NAME}=\"${KEY_VALUE}\"" $CONF_FILE

# restart tftp server
sudo service tftpd-hpa restart
# }}}
fi


# examples for get file
# cd ~/workspace/jz2440v2/output/tftpboot
# echo "hello tftp" > firstme.txt
# cd ~/temp
# tftp 127.0.0.1
# tftp> get firstme.txt
# tftp> quit
# cat firstme.txt

