#!/usr/bin/env bash

# usage:
# source envsetup-busybox.sh

# store the current root path
Q_KERNEL_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)

BUSYBOX_1_7_0="busybox-1.7.0"
BUSYBOX_1_20_0="busybox-1.20.0"
BUSYBOX_VERSION=$BUSYBOX_1_7_0

BUSYBOX_ROOT_PATH="$Q_KERNEL_PATH_CUR/$BUSYBOX_VERSION"
export BUSYBOX_ROOT_PATH

