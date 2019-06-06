#!/bin/sh


if [ $ACTION = "add" ]; then
    mkdir -p /mnt/udisk
    mount /dev/$MDEV /mnt/udisk
else
    umount /mnt/udisk
    rm -rf /mnt/udisk
fi
