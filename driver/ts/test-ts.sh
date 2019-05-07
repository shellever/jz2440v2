#!/bin/sh

# lcd testing
insmod cfbcopyarea.ko
insmod cfbfillrect.ko
insmod cfbimgblt.ko
insmod lcd.ko
#insmod key-input.ko

# ts testing
insmod ts.ko

