# Top Makefile for Uboot
# Copyright (C) 2019 shellever <shellever@163.com>

DIR = .
SUBDIRS = u-boot-1.1.6


all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
u-boot-1.1.6:
	@echo compiling $@...
	cd $@; \
	make 100ask24x0_config; \
	make

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

distclean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

help:
	@echo "==================Top Makefile for Uboot=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all             - build all targets"
	@echo " u-boot-1.1.6    - build bootloader u-boot-1.1.6"
	@echo " archive         - archive image file into output directory"
	@echo " clean           - clean intermediate files"
	@echo " distclean       - clean intermediate files and target"
	@echo " help            - print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "===========================End==============================="

archive:
	@for binfile in `find $$LINUX_ARM_ROOT_PATH/uboot -type f -name u-boot.bin`; do \
		bin_relative_path=`echo "$$binfile" | sed "s#$$LINUX_ARM_ROOT_PATH/uboot/##"`; \
		bin_relative_dir=`echo "$$bin_relative_path" | sed "s#/u-boot.bin##"`; \
		out_bin_dir=$$LINUX_ARM_ROOT_PATH/output/uboot/$$bin_relative_dir; \
		mkdir -p $$out_bin_dir; \
		cp -fp $$binfile $$out_bin_dir; \
	done

.PHONY: all help archive clean distclean

