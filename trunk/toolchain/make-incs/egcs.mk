# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 VDR-NG-EM Project
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

EGCS_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_EGCS),y)
# egcs will only compile with gcc < 6.0.0
ALL_GCCS := $(wildcard $(addsuffix /gcc,$(subst :, ,$(PATH))) \
                      $(addsuffix /gcc-*,$(subst :, ,$(PATH))))
GCC_FOR_EGCS := $(firstword \
                    $(foreach gcc, $(filter-out %/gcc-ar %/gcc-nm %/gcc-ranlib,$(ALL_GCCS)), \
                       $(shell test \
                          $(firstword \
                             $(subst ., ,$(shell $(gcc) -dumpversion))) \
                       -le 5 && echo $(gcc))))

ifeq ($(strip $(GCC_FOR_EGCS)),)
  $(error egcs needs gcc version < 6.0)
endif
endif

# egcs don't know x86_64
HOST_TARGET_FOR_EGCS := $(subst x86_64,i686,$(HOST_TARGET))

EGCS_VERSION := 1.1.2
EGCS_PATCHES_DIR := $(PATCHES_DIR)/egcs/$(EGCS_VERSION)

EGCS_FILE := egcs-$(EGCS_VERSION).tar.bz2
EGCS_DLFILE := $(DOWNLOAD_DIR)/$(EGCS_FILE)
EGCS_URL := ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/egcs-$(EGCS_VERSION)/$(EGCS_FILE)
EGCS_DIR := $(BUILD_DIR)/egcs-$(EGCS_VERSION)
EGCS_BUILD_DIR := $(BUILD_DIR)/egcs.build
EGCS_LIB_DIR := $(PREFIX)/lib/gcc-lib

EGCS_INSTALLED = $(STAGEFILES_DIR)/.egcs_installed
EGCS_BIN = mips-linux-egcs

PACKS_RULES_$(CONFIG_EGCS) += $(EGCS_INSTALLED)

CLEAN_RULES += clean-egcs
DISTCLEAN_RULES += distclean-egcs


#
# download egcs
#

$(EGCS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(EGCS_DLFILE) ] ; then \
	$(WGET) $(EGCS_URL) -O $(EGCS_DLFILE) ; \
	fi );
	$(TOUCH) $(EGCS_DLFILE)

#
# unpack egcs
#

$(STAGEFILES_DIR)/.egcs_unpacked: $(EGCS_DLFILE) \
                                  $(wildcard $(EGCS_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(EGCS_DIR) $(EGCS_BUILD_DIR) $(EGCS_LIB_DIR)
	$(BZCAT) $(EGCS_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_unpacked

#
# patch egcs
#

$(STAGEFILES_DIR)/.egcs_patched: $(STAGEFILES_DIR)/.egcs_unpacked
	$(call patch_package, $(EGCS_DIR), $(EGCS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_patched

#
# configure egcs
#

$(STAGEFILES_DIR)/.egcs_configured: $(STAGEFILES_DIR)/.egcs_patched \
                                        $$(EGCS_DEPS)
	$(LN) -sfn $(PREFIX)/$(UCLIBC_TARGET) $(PREFIX)/$(TARGET)
	-$(RM) -rf $(EGCS_BUILD_DIR)
	$(MKDIR) -p $(EGCS_BUILD_DIR)
	($(CD) $(EGCS_BUILD_DIR) ; PATH='$(PREFIX_BIN):$(PATH)' CC=$(GCC_FOR_EGCS) \
		$(EGCS_DIR)/configure \
			--prefix=$(PREFIX) \
			--build=$(HOST_TARGET_FOR_EGCS) \
			--host=$(HOST_TARGET_FOR_EGCS) \
			--target=$(TARGET) \
			--program-prefix=$(UCLIBC_TARGET)- \
			--with-gnu-as \
			--with-gnu-ld \
			--disable-nls \
			--nfp \
			--enable-languages="c" \
			--with-headers=$(TARGET_ROOT)/usr/include)
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_configured

#
# compile egcs
#

$(STAGEFILES_DIR)/.egcs_compiled: $(STAGEFILES_DIR)/.egcs_configured
	PATH='$(PREFIX_BIN):$(PATH)' CC=$(GCC_FOR_EGCS) \
		$(MAKE) LANGUAGES="c" CFLAGS="-DSYS_SIGLIST_DECLARED -std=gnu90" \
		-C $(EGCS_BUILD_DIR) all-gcc
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_compiled

#
# install egcs
#

$(STAGEFILES_DIR)/.egcs_installed: $(STAGEFILES_DIR)/.egcs_compiled
	if [ -e $(PREFIX)/$(TARGET)/bin/gcc ] ; then \
		$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc  $(PREFIX)/$(TARGET)/bin/gcc.save ; \
	fi
	$(MV) -f $(PREFIX_BIN)/$(TARGET)-gcc  $(PREFIX_BIN)/$(TARGET)-gcc.save
	PATH='$(PREFIX_BIN):$(PATH)' CC=$(GCC_FOR_EGCS) \
		$(MAKE) LANGUAGES="c" CFLAGS="-DSYS_SIGLIST_DECLARED -std=gnu90" \
		-C $(EGCS_BUILD_DIR) install
	$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc  $(PREFIX)/$(TARGET)/bin/egcs
	if [ -e $(PREFIX)/$(TARGET)/bin/gcc.save ] ; then \
		$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc.save  $(PREFIX)/$(TARGET)/bin/gcc; \
	fi
	$(MV) -f $(PREFIX_BIN)/$(TARGET)-gcc  $(PREFIX_BIN)/$(TARGET)-egcs
	$(MV) -f $(PREFIX_BIN)/$(TARGET)-gcc.save  $(PREFIX_BIN)/$(TARGET)-gcc
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_installed

#
# generate rootfs file list if need.
# file list should depend on installed package
#

.PHONY: clean-egcs distclean-egcs

#
# clean package build (build dirs)
# stagefiles should left intact, to support rootfs generation
# without the need of rebuild everthing
#

clean-egcs:
	-$(RM) -rf $(EGCS_LIB_DIR)
	-$(RM) -rf $(EGCS_BUILD_DIR)
	-$(RM) -rf $(EGCS_DIR)

#
# clean everthing else
#

distclean-egcs:
	-$(RM) -f $(STAGEFILES_DIR)/.egcs_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.egcs_patched
	-$(RM) -f $(STAGEFILES_DIR)/.egcs_configured
	-$(RM) -f $(STAGEFILES_DIR)/.egcs_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.egcs_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(EGCS_DLFILE)
endif
