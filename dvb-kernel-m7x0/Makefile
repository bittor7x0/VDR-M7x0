VERSION := 1.1.1
PACKAGE := linuxtv-dvb-$(VERSION)
CVSROOT     := $(shell cat CVS/Root)
RELEASE_TAG := LINUXTV-DVB-$(subst .,_,$(subst -,_,$(VERSION)))

KERNEL_MINOR := $(shell uname -r | sed -e 's/^2\.\(.\).*/\1/')
BUILD_DIR := $(shell pwd)/build-2.$(KERNEL_MINOR)

all:

release dist:
	rm -rf release-tmp $(PACKAGE).tar.gz
	mkdir release-tmp
	( cd release-tmp; cvs -d$(CVSROOT) export -r$(RELEASE_TAG) -d$(PACKAGE) dvb-kernel )
	( cd release-tmp/$(PACKAGE) && rm -rf patches-2.6 analog-2.4 mediafocusII-2.4 build-template )
	find release-tmp -name .cvsignore | xargs rm -v
	( cd release-tmp; tar cjf ../$(PACKAGE).tar.bz2 $(PACKAGE) )
	rm -rf release-tmp
	@echo
	@echo --------------------------------------------------------------------------------------
	@echo
	@echo "driver package: ./$(PACKAGE).tar.bz2"
	@echo
	@echo --------------------------------------------------------------------------------------
	@echo

install:
	$(MAKE) -C $(BUILD_DIR) install

%::
	$(MAKE) -C $(BUILD_DIR) $(MAKECMDGOALS)
	@echo
	@echo --------------------------------------------------------------------------------------
	@echo
	@echo The driver was built in $(BUILD_DIR). Please use 
	@echo
	@echo $(BUILD_DIR)/insmod.sh or
	@echo $(BUILD_DIR)/insmod-bt8xx.sh
	@echo
	@echo "to load and unload the modules for testing. (see README for details)"
	@echo
	@echo --------------------------------------------------------------------------------------
	@echo
