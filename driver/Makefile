# Top Makefile for Driver
# Copyright (C) 2019 shellever <shellever@163.com>

DIR = .
#SUBDIRS = $(shell ls $(DIR))
SUBDIRS = $(shell for file in `ls $(DIR)`; do test -d $$file && echo $$file; done)


all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $(DIR)/$@

test:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

distclean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $(DIR)/$$subdir $@; \
	done

help:
	@echo "==================Top Makefile for Driver=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all       - build all kernel modules"
	@echo " test      - build all tests"
	@echo " archive   - archive bin file into output directory"
	@echo " clean     - clean intermediate files"
	@echo " distclean - clean intermediate files and target"
	@echo " help      - print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "===========================End==============================="

archive:
	@for kofile in `find $$LINUX_ARM_ROOT_PATH/driver -type f \( -name '*.ko' -o -name '*.out' \)`; do \
		ko_relative_path=`echo "$$kofile" | sed "s#$$LINUX_ARM_ROOT_PATH/driver##"`; \
		ko_relative_dir=`dirname $$ko_relative_path`; \
		out_ko_dir=$$LINUX_ARM_ROOT_PATH/output/driver/$$ko_relative_dir; \
		mkdir -p $$out_ko_dir; \
		cp -fp $$kofile $$out_ko_dir; \
	done

.PHONY: all test help archive clean distclean

