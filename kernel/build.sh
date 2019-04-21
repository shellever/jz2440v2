#!/usr/bin/env bash

#
# build script for kernel 
#

function print_usage()
{
    echo
    echo "Usage: ./build.sh [n]"
    echo " 1 - linux-2.6.22.6"
    echo " 2 - linux-3.4.2"
    echo
}

function show_option_menu()
{
cat << EOF
`echo -e "\033[35m 1) linux-2.6.22.6\033[0m"`
`echo -e "\033[35m 2) linux-3.4.2\033[0m"`
EOF
read -p "please enter your choice [n]: " choice
}

# [ $# -gt 1 ] && {
    # print_usage
    # exit 1
# }

# [ $# -eq 0 ] && {
# cat << EOF
# `echo -e "\033[35m 1) apple\033[0m"`
# `echo -e "\033[35m 2) meizu\033[0m"`
# `echo -e "\033[35m 3) xiaomi\033[0m"`
# EOF
# read -p "please enter your choice [n]: " choice
# }

# [ $# -eq 0 ] || {
    # choice=$1
# }

#show_option_menu

case $# in
    0) show_option_menu;;
    1) choice=$1;;
    *) print_usage; exit 1;;
esac

case $choice in
    1)
        current="linux-2.6.22.6"
        echo "current: $current"
        [ ! -d $current ] && echo "$current no exist now"; exit 1
        cd $current
        # [ ! -f .config ] && cp -fp config_ok .config
        make jz2440v2_defconfig
        make uImage
        cd -
        ;;
    2)
        current="linux-3.4.2"
        echo "current: $current"
        ;;
    *)
        echo "invalid choice, return"
        exit 1
        ;;
esac

echo "compile finish"


