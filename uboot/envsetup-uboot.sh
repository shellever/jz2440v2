#!/usr/bin/env bash

# usage:
# source envsetup-uboot.sh

# store the current script root path
Q_UBOOT_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)

UBOOT_VERSION="u-boot-1.1.6"
#UBOOT_VERSION="u-boot-2012.04.01"
UBOOT_ROOT_PATH="$Q_UBOOT_PATH_CUR/$UBOOT_VERSION"
export UBOOT_ROOT_PATH

