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
# $Id: binutils.mk 401 2009-03-26 12:29:09Z andreas $
#

BINUTILS_VERSION := 2.20.1
BINUTILS_PATCHES_DIR := $(PATCHES_DIR)/binutils/$(BINUTILS_VERSION)

BINUTILS_DIR := $(BUILD_DIR)/binutils-$(BINUTILS_VERSION)
BINUTILS_FILE := binutils-$(BINUTILS_VERSION).tar.bz2
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
                                 $(BINUTILS_PATCHES_DIR)/*.patch
	-$(RM) -rf $(BINUTILS_DIR)
	$(BZCAT) $(BINUTILS_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
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

$(STAGEFILES_DIR)/.binutils_configured: $(STAGEFILES_DIR)/.binutils_patched
	-$(RM) -rf $(BINUTILS_BUILD_DIR)
	$(MKDIR) -p $(BINUTILS_BUILD_DIR)
	($(CD) $(BINUTILS_BUILD_DIR) ; \
		$(BINUTILS_DIR)/configure \
			--prefix=$(PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--target=$(UCLIBC_TARGET) \
			--disable-werror \
			--disable-nls )
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
