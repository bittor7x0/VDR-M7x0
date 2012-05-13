# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012 VDR-NG-EM Project
#
# More information can be found in the files COPYING and README.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# The project's page is at http://www.assembla.com/spaces/VDR-M7x0
# Our English forum is at  http://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  http://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBJS_DEPS = $(BASE_BUILD_STAGEFILE)

LIBJS_VERSION := 1.8.0-rc1
LIBJS_PATCHES_DIR := $(PATCHES_DIR)/libjs/$(LIBJS_VERSION)

LIBJS_FILE := js-$(LIBJS_VERSION).tar.gz
LIBJS_DLFILE := $(DOWNLOAD_DIR)/$(LIBJS_FILE)
LIBJS_URL := http://ftp.mozilla.org/pub/mozilla.org/js/$(LIBJS_FILE)
LIBJS_DIR := $(BUILD_DIR)/libjs-$(LIBJS_VERSION)

LIBJS_INSTALLED = $(STAGEFILES_DIR)/.libjs_installed

PACKS_RULES_$(CONFIG_LIBJS) += $(LIBJS_INSTALLED)
FILE_LISTS_$(CONFIG_LIBJS) += libjs.lst

CLEAN_RULES += clean-libjs
DISTCLEAN_RULES += distclean-libjs

#
# download libjs
#

$(LIBJS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBJS_DLFILE) ] ; then \
	$(WGET) $(LIBJS_URL) -O $(LIBJS_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBJS_DLFILE)

#
# unpack libjs
#

$(STAGEFILES_DIR)/.libjs_unpacked: $(LIBJS_DLFILE) \
                                           $(wildcard $(LIBJS_PATCHES_DIR)/*.patch) \
                                           $$(LIBJS_DEPS)
	-$(RM) -rf $(LIBJS_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBJS_DLFILE)
	$(MV) $(BUILD_DIR)/js $(LIBJS_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libjs_unpacked

#
# patch libjs
#

$(STAGEFILES_DIR)/.libjs_patched: $(STAGEFILES_DIR)/.libjs_unpacked
	$(call patch_package, $(LIBJS_DIR), $(LIBJS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libjs_patched

#
# compile libjs
#

$(STAGEFILES_DIR)/.libjs_compiled: $(STAGEFILES_DIR)/.libjs_patched
	PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) -C $(LIBJS_DIR)/src -f Makefile.ref BUILD_OPT=1 PREBUILT_CPUCFG=1 PREBUILT_KWGEN=1 JSCPUCFG_DEFINES='-DIS_BIG_ENDIAN' CC=$(UCLIBC_CC) LD=$(UCLIBC_LD)
	$(TOUCH) $(STAGEFILES_DIR)/.libjs_compiled

#
# install libjs
#

$(STAGEFILES_DIR)/.libjs_installed: $(STAGEFILES_DIR)/.libjs_compiled
	# readme http://ebixio.com/blog/2010/07/31/how-to-install-libjs-spidermonkey/
	PATH="$(PREFIX_BIN):$(PATH)" DESTDIR=$(TARGET_ROOT)/usr BUILD_OPT=1 PREBUILT_CPUCFG=1 PREBUILT_KWGEN=1 JSCPUCFG_DEFINES='-DIS_BIG_ENDIAN' CC=$(UCLIBC_CC) LD=$(UCLIBC_LD) $(MAKE) -C $(LIBJS_DIR)/src install
	$(TOUCH) $(STAGEFILES_DIR)/.libjs_installed


$(FILELIST_DIR)/libjs.lst: $(STAGEFILES_DIR)/.libjs_installed
	$(TOUCH) $(FILELIST_DIR)/libjs.lst

.PHONY: clean-libjs distclean-libjs

clean-libjs:
	-$(RM) -rf $(LIBJS_DIR)

#
# clean everthing else
#

distclean-libjs:
	-$(RM) -f $(STAGEFILES_DIR)/.libjs_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libjs_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libjs_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libjs_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBJS_DLFILE)
endif
