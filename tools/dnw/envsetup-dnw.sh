#!/usr/bin/env bash

# usage:
# source envsetup-dnw.sh

# store the current script root path
Q_DNW_PATH_CUR=$(cd $(dirname "${BASH_SOURCE}") && pwd)
# store the current working directory
Q_WORK_DIR_CUR=$(pwd)


# ===>enter
cd $Q_DNW_PATH_CUR

export DNW_PATH="$Q_DNW_PATH_CUR/bin"
DNW_SCRIPT="$Q_DNW_PATH_CUR/script"
export PATH=$DNW_SCRIPT:$PATH

#[ ! -f /usr/bin/dnw ] && sudo ln -s $DNW_PATH/dnw /usr/bin/dnw

# <===exit
cd $Q_WORK_DIR_CUR


# usage:
# dnw <file> <address>

