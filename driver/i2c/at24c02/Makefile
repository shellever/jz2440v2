CROSS_COMPILE = arm-linux-
# use exported variable from source envsetup.sh
CC = $(CROSS_COMPILE)gcc

CFLAGS += -Wall -O2
CFLAGS += -I.
# for i2c-dev
CFLAGS += -I$(KERNEL_ROOT_PATH)/include


OBJS = i2c-drv.o at24c02.o at24c02-test.o
SRCS := $(OBJS:.o=.c)

ALL := at24c02-test.out


all: $(ALL)

$(ALL): .depend $(OBJS)
	$(CC) -o $@ $(OBJS)


# rules define
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# depend define
.depend: $(SRCS)
	@rm -f $@
	@for f in $(SRCS); do \
		$(CC) $(CFLAGS) -MM $$f >> $@; \
	done

copy:
	mkdir -p $(ROOTFS_ROOT_PATH)/test/i2c/at24c02
	cp -fp *.out $(ROOTFS_ROOT_PATH)/test/i2c/at24c02/

clean:
	rm -f $(OBJS) .depend

distclean: clean
	rm -f $(ALL)

.PHONY: all test clean distclean copy

