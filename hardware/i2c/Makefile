# Top Makefile for Hardware I2C 
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
	@echo "==================Top Makefile for Hardware I2C=================="
	@echo "Copyright (C) 2019 Shellever <shellever@163.com>"
	@echo "supported targets below:"
	@echo
	@echo " all       - build all targets"
	@echo " clean     - clean intermediate files"
	@echo " distclean - clean intermediate files and target"
	@echo " help      - print help information"
	@echo
	@echo "to make a target, do 'make <target>'"
	@echo "=================================================================="

.PHONY: all help clean distclean

