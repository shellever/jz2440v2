# Top Makefile for Hardware
# Copyright (C) 2019 shellever <shellever@163.com>

DIR = .
#SUBDIRS = $(shell ls $(DIR))
SUBDIRS = $(shell for file in `ls $(DIR)`; do test -d $$file && echo $$file; done)


all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $(DIR)/$@

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

distclean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

help:
	@echo "==================Top Makefile for Hardware=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all       - build all targets"
	@echo " archive   - archive bin file into output directory"
	@echo " clean     - clean intermediate files"
	@echo " distclean - clean intermediate files and target"
	@echo " help      - print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "===========================End==============================="

archive:
	@for binfile in `find $$LINUX_ARM_ROOT_PATH/hardware -type f -name *.bin`; do \
		bin_relative_path=`echo "$$binfile" | sed "s#$$LINUX_ARM_ROOT_PATH/hardware##"`; \
		bin_relative_dir=`dirname $$bin_relative_path`; \
		out_bin_dir=$$LINUX_ARM_ROOT_PATH/output/hardware/$$bin_relative_dir; \
		mkdir -p $$out_bin_dir; \
		cp -fp $$binfile $$out_bin_dir; \
	done

.PHONY: all help archive clean distclean

