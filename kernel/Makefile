# Top Makefile for Kernel
# Copyright (C) 2019 shellever <shellever@163.com>

DIR = .
SUBDIRS = linux-2.6.22.6


all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
linux-2.6.22.6:
	@echo compiling linux-2.6.22.6; \
	cd $@; \
	[ ! -f .config ] && make jz2440v2_defconfig; \
	make uImage

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

distclean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

help:
	@echo "==================Top Makefile for Kernel=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all       		- build all targets"
	@echo " linux-2.6.22.6	- build kernel linux-2.6.22.6"
	@echo " archive   		- archive image file into output directory"
	@echo " clean     		- clean intermediate files"
	@echo " distclean 		- clean intermediate files and target"
	@echo " help      		- print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "===========================End==============================="

archive:
	@for imgfile in `find $$LINUX_ARM_ROOT_PATH/kernel -type f -name uImage`; do \
		img_relative_path=`echo "$$imgfile" | sed "s#$$LINUX_ARM_ROOT_PATH/kernel/##"`; \
		img_relative_dir=`echo "$$img_relative_path" | sed "s#/arch/arm/boot/uImage##"`; \
		out_img_dir=$$LINUX_ARM_ROOT_PATH/output/kernel/$$img_relative_dir; \
		mkdir -p $$out_img_dir; \
		cp -fp $$imgfile $$out_img_dir; \
	done

.PHONY: all help archive clean distclean

