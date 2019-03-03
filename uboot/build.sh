#!/usr/bin/env bash

#
# build script for u-boot
#

function print_usage()
{
    echo
    echo "Usage: ./build.sh [n]"
    echo " 1 - u-boot-1.1.6"
    echo " 2 - u-boot-2012.04.01"
    echo
}

function show_option_menu()
{
cat << EOF
`echo -e "\033[35m 1) u-boot-1.1.6\033[0m"`
`echo -e "\033[35m 2) u-boot-2012.04.01\033[0m"`
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
        current="u-boot-1.1.6"
        echo "current: $current"
        ;;
    2)
        current="u-boot-2012.04.01"
        echo "current: $current"
        ;;
    *)
        echo "invalid choice, return"
        exit 1
        ;;
esac

[ ! -d $current ] && echo "$current no exist now"; exit 1
cd $current
#make distclean
make 100ask24x0_config
make


