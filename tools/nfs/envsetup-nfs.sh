#!/usr/bin/env bash

# usage:
# source envsetup-nfs.sh

# store the current script root path
Q_NFS_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# exported directory of nfs
NFS_ROOT_PATH="$LINUX_ARM_ROOT_PATH/output/nfsroot"
if [ ! -e $NFS_ROOT_PATH ]; then
    mkdir -p $NFS_ROOT_PATH
fi
export NFS_ROOT_PATH


if [ ! -e /etc/exports ]; then
# installation {{{
sudo apt-get install nfs-kernel-server
# configure the exported directory
sudo cat >> /etc/exports << EOF

# nfs workspace for jz2440
# `date "+%m/%d/%Y %H:%M%S"`
${NFS_ROOT_PATH} *(rw,sync,no_root_squash)

EOF

# restart nfs server
sudo service nfs-kernel-server restart

# show the nfs server status
#service nfs-kernel-server status
# show the exported directory of nfs server
#showmount -e
# }}}
fi

# examples for mounting
# 1. mounted into directory /mnt/nfs in embedded device
# mount -t nfs -o nolock 192.168.31.212:/home/linuxfor/workspace/jz2440v2/output/nfsroot /mnt/nfs
# umount /mnt/nfs

