LUA_DIR ?= $(ROOTFS_ROOT_PATH)

CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc

CFLAGS = -I$(LUA_DIR)/include
CFLAGS += -Wall -O2
CFLAGS += -std=gnu99
LUALIBS += -L$(LUA_DIR)/lib -llua

TARGET = main
OBJS = main.o


all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "building $@..."
	$(CC) $(CFLAGS) $(LUALIBS) -o $@ $^

%.o: %.c
	@echo "compiling $@..."
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJS)

distclean: clean
	rm -rf $(TARGET)

.PHONY: all clean distclean

