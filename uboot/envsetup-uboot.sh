#!/usr/bin/env bash

# usage:
# source envsetup-uboot.sh

# store the current script root path
Q_UBOOT_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)

UBOOT_1_1_6="u-boot-1.1.6"
UBOOT_2012_04_01="u-boot-2012.04.01"
UBOOT_VERSION=$UBOOT_1_1_6

UBOOT_ROOT_PATH="$Q_UBOOT_PATH_CUR/$UBOOT_VERSION"
export UBOOT_ROOT_PATH

