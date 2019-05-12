LUA_DIR ?= $(ROOTFS_ROOT_PATH)

CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc

CFLAGS = -I$(LUA_DIR)/include
#CFLAGS += -L$(LUA_DIR)/lib -llua
#CFLAGS += -fPIC
#CFLAGS += -lm -ldl
#CFLAGS += -fPIC -shared
#CFLAGS += -Wall -O2
CFLAGS += -std=gnu99

LIB = mylib.so
OBJS = mylib.o

all: $(LIB)

$(LIB): $(OBJS)
	@echo "generating $(LIB) ..."
	$(CC) -fPIC -shared -o $@ $^

%.o: %.c
	@echo "compiling ..."
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJS)

distclean: clean
	rm -rf $(LIB)

.PHONY: all clean distclean

