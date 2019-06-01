#!/bin/sh

# remove module
rmmod debug_zprintk
rmmod proc_zmsg

# install module
insmod proc-zmsg.ko
insmod debug-zprintk.ko

# execute test program
./debug-zprintk-test.out

# view the log
ls -l /proc/zmsg
cat /proc/zmsg
