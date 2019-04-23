# Top Makefile for rootfs
# Copyright (C) 2019 shellever <shellever@163.com>

ROOTFSDIR = rootfs

ROOTFS_YAFFS2 = rootfs.yaffs2
ROOTFS_JFFS2 = rootfs.jffs2


DEV_NULL = $(ROOTFSDIR)/dev/null
DEV_CONSOLE = $(ROOTFSDIR)/dev/console
ALL_DEVS = $(DEV_NULL) $(DEV_CONSOLE)

DIR_DEV = $(ROOTFSDIR)/dev
DIR_SYS = $(ROOTFSDIR)/sys
DIR_PROC = $(ROOTFSDIR)/proc
MKNOD_PRE_DIRS = $(DIR_DEV) $(DIR_PROC) $(DIR_SYS)

MKFS_PRE_DEVS = $(MKNOD_PRE_DIRS) $(ALL_DEVS)
ALL := $(ROOTFS_YAFFS2) $(ROOTFS_JFFS2)


all: $(ALL)

$(ROOTFS_YAFFS2): $(MKFS_PRE_DEVS)
	@echo "building $@..."
	@mkyaffs2image $(ROOTFSDIR) $@

$(ROOTFS_JFFS2): $(MKFS_PRE_DEVS)
	@echo "building $@..."
	@mkfs.jffs2 -n -s 2048 -e 128KiB -d $(ROOTFSDIR) -o $@


$(DEV_NULL):
	@echo "mknod $@..."
	@sudo mknod -m 666 $@ c 1 3

$(DEV_CONSOLE):
	@echo "mknod $@..."
	@sudo mknod -m 600 $@ c 5 1

$(MKNOD_PRE_DIRS):
	@echo "mkdir $@..."
	@mkdir -p $@


clean:
	rm -rf $(MKFS_PRE_DEVS)

distclean: clean
	rm -rf $(ROOTFS_YAFFS2) $(ROOTFS_JFFS2)

.PHONY: all clean distclean

