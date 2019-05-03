KERN_DIR ?= $(KERNEL_ROOT_PATH)
#KERN_DIR ?= $(LINUX_ARM_ROOT_PATH)/kernel/linux-2.6.22.6-usb
PWD := $(shell pwd)

CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc

# module objects
obj-m += usb-mouse-key.o
# test binary file
test-out :=


all:
	make -C $(KERN_DIR) M=$(PWD) modules


test: $(test-out)

$(test-out): %.out: %.o
	@echo "linking ..."
	$(CC) -o $@ $^

%.o: %.c
	@echo "compiling ..."
	$(CC) $(CFLAGS) -c -o $@ $^


clean:
	@echo "cleaning ..."
	rm -rf *.o *.symvers .*.d .*.cmd *.mod.c .tmp_versions

distclean: clean
	@echo "distcleaning ..."
	rm -f *.ko *.out

.PHONY: all test clean distclean

