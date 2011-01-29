# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#

EGCS_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_EGCS),y)
# egcs will only compile with gcc < 4.0.0
ALL_GCCS := $(wildcard $(addsuffix /gcc,$(subst :, ,$(PATH))) \
                      $(addsuffix /gcc-*,$(subst :, ,$(PATH))))
GCC_FOR_EGCS := $(firstword \
                    $(foreach gcc, $(ALL_GCCS), \
                       $(shell test \
                          $(firstword \
                             $(subst ., ,$(shell $(gcc) -dumpversion))) \
                       -le 3 && echo $(gcc))))

ifeq ($(strip $(GCC_FOR_EGCS)),)
  $(error egcs needs gcc version < 4.0)
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
	-$(RM) -rf $(EGCS_DIR)
	$(BZCAT) $(EGCS_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_unpacked

#
# patch egcs
#

$(STAGEFILES_DIR)/.egcs_patched: $(STAGEFILES_DIR)/.egcs_unpacked
	$(call patch_package, $(EGCS_DIR), $(EGCS_PATCHES_DIR)/common)
	$(call patch_package, $(EGCS_DIR), $(EGCS_PATCHES_DIR)/host/$(HOST_BS))
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
		$(MAKE) LANGUAGES="c" CFLAGS=-DSYS_SIGLIST_DECLARED \
		-C $(EGCS_BUILD_DIR) all-gcc
	$(TOUCH) $(STAGEFILES_DIR)/.egcs_compiled

#
# install egcs
#

$(STAGEFILES_DIR)/.egcs_installed: $(STAGEFILES_DIR)/.egcs_compiled
	$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc  $(PREFIX)/$(TARGET)/bin/gcc.save
	$(MV) -f $(PREFIX_BIN)/$(TARGET)-gcc  $(PREFIX_BIN)/$(TARGET)-gcc.save
	PATH='$(PREFIX_BIN):$(PATH)' CC=$(GCC_FOR_EGCS) \
		$(MAKE) LANGUAGES="c" CFLAGS=-DSYS_SIGLIST_DECLARED \
		-C $(EGCS_BUILD_DIR) install
	$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc  $(PREFIX)/$(TARGET)/bin/egcs
	$(MV) -f $(PREFIX)/$(TARGET)/bin/gcc.save  $(PREFIX)/$(TARGET)/bin/gcc
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
