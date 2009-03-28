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
# $Id: ncurses.mk 374 2008-09-01 18:22:42Z andreas $
#

NCURSES_DEPS = $(BASE_BUILD_STAGEFILE)

NCURSES_VERSION := 5.6
NCURSES_PATCHES_DIR := $(PATCHES_DIR)/ncurses/$(NCURSES_VERSION)

NCURSES_FILE := ncurses-$(NCURSES_VERSION).tar.gz
NCURSES_DLFILE := $(DOWNLOAD_DIR)/$(NCURSES_FILE)
NCURSES_DIR := $(BUILD_DIR)/ncurses-$(NCURSES_VERSION)
NCURSES_BUILD_DIR := $(BUILD_DIR)/ncurses.build
NCURSES_URL := ftp://ftp.gnu.org/gnu/ncurses/$(NCURSES_FILE)

NCURSES_INSTALLED = $(STAGEFILES_DIR)/.ncurses_installed

PACKS_RULES_$(CONFIG_NCURSES) += $(NCURSES_INSTALLED)
FILE_LISTS_$(CONFIG_NCURSES) += ncurses.lst

CLEAN_RULES += clean-ncurses
DISTCLEAN_RULES += distclean-ncurses

#
# download ncurses
#

$(NCURSES_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NCURSES_DLFILE) ] ; then \
	$(WGET) $(NCURSES_URL) -O $(NCURSES_DLFILE) ; \
	fi );
	$(TOUCH) $(NCURSES_DLFILE)

#
# unpack ncurses
#

$(STAGEFILES_DIR)/.ncurses_unpacked: $(NCURSES_DLFILE) \
                                     $(wildcard $(NCURSES_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(NCURSES_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(NCURSES_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.ncurses_unpacked

#
# patch ncurses
#

$(STAGEFILES_DIR)/.ncurses_patched: $(STAGEFILES_DIR)/.ncurses_unpacked
	$(call patch_package, $(NCURSES_DIR), $(NCURSES_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ncurses_patched
#
# configure ncurses
#

$(STAGEFILES_DIR)/.ncurses_configured: $(STAGEFILES_DIR)/.ncurses_patched \
                                       $$(NCURSES_DEPS)
	-$(RM) -rf $(NCURSES_BUILD_DIR)
	$(MKDIR) -p $(NCURSES_BUILD_DIR)
	($(CD) $(NCURSES_BUILD_DIR) ; $(UCLIBC_ENV) \
		$(NCURSES_DIR)/configure \
			--prefix=/usr \
			--libdir=/lib \
			--with-install-prefix=$(TARGET_ROOT) \
			--with-shared \
			--target=$(UCLIBC_TARGET) \
			--host=$(UCLIBC_TARGET) \
			--without-ada \
			--without-progs \
			--without-debug \
			--without-profile \
			--without-dbmalloc \
			--without-dmalloc \
			--without-gpm \
			--disable-rpath)
	$(TOUCH) $(STAGEFILES_DIR)/.ncurses_configured

#
# compile ncurses
#

$(STAGEFILES_DIR)/.ncurses_compiled: $(STAGEFILES_DIR)/.ncurses_configured
	$(UCLIBC_ENV) $(MAKE) -C $(NCURSES_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.ncurses_compiled

#
# install ncurses
#

$(STAGEFILES_DIR)/.ncurses_installed: $(STAGEFILES_DIR)/.ncurses_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(NCURSES_BUILD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.ncurses_installed

$(FILELIST_DIR)/ncurses.lst: $(STAGEFILES_DIR)/.ncurses_installed
	$(TOUCH) $(FILELIST_DIR)/ncurses.lst

.PHONY: clean-ncurses distclean-ncurses

clean-ncurses:
	-$(RM) -rf $(NCURSES_BUILD_DIR)
	-$(RM) -rf $(NCURSES_DIR)

distclean-ncurses:
	-$(RM) -f $(STAGEFILES_DIR)/.ncurses_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ncurses_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ncurses_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ncurses_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ncurses_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NCURSES_DLFILE)
endif
