#!/usr/bin/env bash

# usage:
# source envsetup-kernel.sh

# store the current root path
Q_KERNEL_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)

KERNEL_VERSION="linux-2.6.22.6"
#KERNEL_VERSION="linux-3.4.2"
KERNEL_ROOT_PATH="$Q_KERNEL_PATH_CUR/$KERNEL_VERSION"
export KERNEL_ROOT_PATH

