# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2024 VDR-NG-EM Project
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here
BINUTILS_DEPS = $(GMP_HOSTINSTALLED) $(MPFR_HOSTINSTALLED) $(MPC_HOSTINSTALLED) $(ISL_HOSTINSTALLED)

BINUTILS_VERSION := 2.41
BINUTILS_PATCHES_DIR := $(PATCHES_DIR)/binutils/$(BINUTILS_VERSION)

BINUTILS_DIR := $(BUILD_DIR)/binutils-$(BINUTILS_VERSION)
BINUTILS_FILE := binutils-$(BINUTILS_VERSION).tar.xz
BINUTILS_DLFILE := $(DOWNLOAD_DIR)/$(BINUTILS_FILE)
BINUTILS_BUILD_DIR := $(BUILD_DIR)/binutils.build
BINUTILS_URL := ftp://ftp.gnu.org/gnu/binutils/$(BINUTILS_FILE)

BINUTILS_INSTALLED = $(STAGEFILES_DIR)/.binutils_installed

BASE_RULES_y += $(BINUTILS_INSTALLED)
CLEAN_RULES += clean-binutils
DISTCLEAN_RULES += distclean-binutils

#
# download needed filesets
#

$(BINUTILS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(BINUTILS_DLFILE) ] ; then \
	$(WGET) $(BINUTILS_URL) -O $(BINUTILS_DLFILE) ; \
	fi );
	$(TOUCH) $(BINUTILS_DLFILE)

#
# unpack binutils
#

$(STAGEFILES_DIR)/.binutils_unpacked: $(BINUTILS_DLFILE) \
                                 $(wildcard $(BINUTILS_PATCHES_DIR)/*.patch) \
                                 $$(BINUTILS_DEPS)
	-$(RM) -rf $(BINUTILS_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(BINUTILS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.binutils_unpacked

#
# patch binutils
#

$(STAGEFILES_DIR)/.binutils_patched: $(STAGEFILES_DIR)/.binutils_unpacked
	$(call patch_package, $(BINUTILS_DIR), $(BINUTILS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.binutils_patched
#
# configure binutils
#

$(STAGEFILES_DIR)/.binutils_configured: $(STAGEFILES_DIR)/.binutils_patched \
                                        $$(ZSTD_HOSTINSTALLED)
	-$(RM) -rf $(BINUTILS_BUILD_DIR)
	$(MKDIR) -p $(BINUTILS_BUILD_DIR)
	($(CD) $(BINUTILS_BUILD_DIR) ; \
		$(BINUTILS_DIR)/configure \
			--prefix=$(PREFIX) \
			--build=$(HOST_TARGET) \
			--host=$(HOST_TARGET) \
			--target=$(UCLIBC_TARGET) \
			--with-sysroot=$(TARGET_ROOT) \
			--with-gmp=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-isl=$(PREFIX) \
			--with-zstd=$(PREFIX) \
			--enable-compressed-debug-sections=all \
			--enable-default-compressed-debug-sections-algorithm=zstd \
			--enable-plugins \
			--enable-deterministic-archives \
			--disable-sim \
			--disable-gdb \
			--disable-multilib \
			--disable-werror \
			--disable-nls)
	$(TOUCH) $(STAGEFILES_DIR)/.binutils_configured

#
# compile binutils
#

$(STAGEFILES_DIR)/.binutils_compiled: $(STAGEFILES_DIR)/.binutils_configured
	$(MAKE) -C $(BINUTILS_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.binutils_compiled

#
# install binutils
#

$(STAGEFILES_DIR)/.binutils_installed: $(STAGEFILES_DIR)/.binutils_compiled
	$(MAKE) -C $(BINUTILS_BUILD_DIR) install
	if [ -e $(PREFIX)/$(UCLIBC_TARGET)/bin/as.orig ] ; then \
		rm -f   $(PREFIX)/$(UCLIBC_TARGET)/bin/as.orig ; \
	fi
	$(TOUCH) $(STAGEFILES_DIR)/.binutils_installed

.PHONY: clean-binutils distclean-binutils

clean-binutils:
	-$(RM) -rf $(BINUTILS_DIR)
	-$(RM) -rf $(BINUTILS_BUILD_DIR)

distclean-binutils:
	-$(RM) -f $(STAGEFILES_DIR)/.binutils_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.binutils_patched
	-$(RM) -f $(STAGEFILES_DIR)/.binutils_configured
	-$(RM) -f $(STAGEFILES_DIR)/.binutils_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.binutils_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BINUTILS_DLFILE)
endif
