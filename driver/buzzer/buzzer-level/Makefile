KERN_DIR ?= $(KERNEL_ROOT_PATH)
PWD := $(shell pwd)

CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc

# module objects
obj-m += buzzer-level.o
# test binary file
test-out := buzzer-level-test.out


all:
	make -C $(KERN_DIR) M=$(PWD) modules


test: $(test-out)

$(test-out): %.out: %.o
	@echo "linking ..."
	$(CC) -o $@ $^

%.o: %.c
	@echo "compiling ..."
	$(CC) $(CFLAGS) -c -o $@ $^


copy:
	mkdir -p $(ROOTFS_ROOT_PATH)/test/buzzer/buzzer-level
	cp -fp *.ko *.out $(ROOTFS_ROOT_PATH)/test/buzzer/buzzer-level/

clean:
	@echo "cleaning ..."
	rm -rf *.o *.symvers .*.d .*.cmd *.mod.c .tmp_versions

distclean: clean
	@echo "distcleaning ..."
	rm -f *.ko *.out

.PHONY: all copy test clean distclean

