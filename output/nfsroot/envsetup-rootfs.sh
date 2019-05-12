#!/usr/bin/env bash

# usage:
# source envsetup-rootfs.sh

# store the current root path
Q_ROOTFS_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)

ROOTFS_DIR="rootfs"

ROOTFS_ROOT_PATH="$Q_ROOTFS_PATH_CUR/$ROOTFS_DIR"
export ROOTFS_ROOT_PATH

