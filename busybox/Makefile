# Top Makefile for Busybox
# Copyright (C) 2019 shellever <shellever@163.com>


BUSYBOX_170 := busybox-1.7.0
SQLITE_328 := sqlite-autoconf-3280000
LUA_535 := lua-5.3.5

DIR = .
SUBDIRS = $(BUSYBOX_170)
SUBDIRS += $(SQLITE_328)
SUBDIRS += $(LUA_535)


all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(BUSYBOX_170):
	@echo compiling $@...
	cd $@; \
	install_dir=$$LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs-`date +"%Y%m%d%H%M%S"`; \
	mkdir -p $$install_dir; \
	make; \
	make CONFIG_PREFIX=$$install_dir install

$(SQLITE_328):
	@echo compiling $@...
	cd $@; \
	install_dir=$$LINUX_ARM_ROOT_PATH/output/busybox/sqlite/$@; \
	mkdir -p $$install_dir; \
	./configure --host=arm-linux --prefix=$$install_dir; \
	make; \
	make install

$(LUA_535):
	@echo compiling $@...
	cd $@; \
	install_dir=$$LINUX_ARM_ROOT_PATH/output/busybox/lua/$@; \
	mkdir -p $$install_dir; \
	make arm-linux; \
	make INSTALL_TOP=$$install_dir install; \
	ls $$install_dir


# one-dimensional operation
# such as:
# make clean
# make distclean
clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

distclean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

# two-dimensional operation
# such as:
# make clean/busybox-1.7.0
# make distclean/busybox-1.7.0
CLEANDIRS := $(addprefix clean/, $(SUBDIRS))

$(CLEANDIRS):
	target=`echo $@ | awk -F/ '{print $$1}'`; \
	subdir=`echo $@ | awk -F/ '{print $$2}'`; \
	make -C $(DIR)/$$subdir $$target

DISTCLEANDIRS := $(addprefix distclean/, $(SUBDIRS))

$(DISTCLEANDIRS):
	target=`echo $@ | awk -F/ '{print $$1}'`; \
	subdir=`echo $@ | awk -F/ '{print $$2}'`; \
	make -C $(DIR)/$$subdir $$target

.PHONY: $(CLEANDIRS) $(DISTCLEANDIRS)


help:
	@echo "==================Top Makefile for Busybox=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all                        - build all targets"
	@echo " busybox-1.7.0              - build filesystem busybox-1.7.0"
	@echo " sqlite-autoconf-3280000    - build database sqlite-autoconf-3280000"
	@echo " lua-5.3.5                  - build database lua-5.3.5"
	@echo " clean                      - clean intermediate files"
	@echo " distclean                  - clean intermediate files and target"
	@echo " help                       - print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "===========================End==============================="


.PHONY: all help clean distclean

