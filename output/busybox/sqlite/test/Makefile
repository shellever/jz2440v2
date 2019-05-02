CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc

SQLITE_INSTALL_DIR := $(LINUX_ARM_ROOT_PATH)/output/busybox/sqlite/sqlite-autoconf-3280000
CPPFLAGS = -I $(SQLITE_INSTALL_DIR)/include
CFLAGS += -L $(SQLITE_INSTALL_DIR)/lib
CFLAGS += -lsqlite3

# test binary file
test-out := sqlite-test.out


all: $(test-out)

$(test-out): %.out: %.o
	@echo "linking ..."
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "compiling ..."
	$(CC) $(CPPFLAGS) -c -o $@ $^


clean:
	@echo "cleaning ..."
	rm -rf $(wildcard *.o)

distclean: clean
	@echo "distcleaning ..."
	rm -f $(wildcard *.out)

.PHONY: all clean distclean

