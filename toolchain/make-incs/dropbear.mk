# Copyright (C) 2008,2009 gambler - the open7x0.org group
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
# The author can be reached at gambler@open7x0.org
#
# The project's page is at http://www.open7x0.org
#

ifeq ($(CONFIG_DROPBEAR),y)
ifneq ($(CONFIG_ZLIB),y)
   $(error dependency error: dropbear needs zlib enabled)
endif
endif

DROPBEAR_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

DROPBEAR_VERSION := 0.52
DROPBEAR_PATCHES_DIR := $(PATCHES_DIR)/dropbear/$(DROPBEAR_VERSION)

DROPBEAR_FILE := dropbear-$(DROPBEAR_VERSION).tar.bz2
DROPBEAR_DLFILE := $(DOWNLOAD_DIR)/$(DROPBEAR_FILE)
DROPBEAR_DIR := $(BUILD_DIR)/dropbear-$(DROPBEAR_VERSION)
DROPBEAR_URL := http://matt.ucc.asn.au/dropbear/releases/$(DROPBEAR_FILE)
DROPBEAR_INSTALLED = $(STAGEFILES_DIR)/.dropbear_installed

PACKS_RULES_$(CONFIG_DROPBEAR) += $(DROPBEAR_INSTALLED)
FILE_LISTS_$(CONFIG_DROPBEAR) += dropbear.lst

CLEAN_RULES += clean-dropbear
DISTCLEAN_RULES += distclean-dropbear

#
# download dropbear
#

$(DROPBEAR_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DROPBEAR_DLFILE) ] ; then \
	$(WGET) $(DROPBEAR_URL) -O $(DROPBEAR_DLFILE) ; \
	fi );
	$(TOUCH) $(DROPBEAR_DLFILE)


#
# unpack dropbear
#

$(STAGEFILES_DIR)/.dropbear_unpacked: $(DROPBEAR_DLFILE) $$(DROPBEAR_DEPS) $(wildcard $(DROPBEAR_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(DROPBEAR_DIR)
	$(BZCAT) $(DROPBEAR_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_unpacked

#
# patch dropbear
#

$(STAGEFILES_DIR)/.dropbear_patched: $(STAGEFILES_DIR)/.dropbear_unpacked
	$(call patch_package, $(DROPBEAR_DIR), $(DROPBEAR_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_patched

#
# configure dropbear
#

$(STAGEFILES_DIR)/.dropbear_configured: $(STAGEFILES_DIR)/.dropbear_patched 
	($(CD) $(DROPBEAR_DIR) ; $(UCLIBC_ENV) \
	    $(DROPBEAR_DIR)/configure \
		--prefix=$(TARGET_ROOT) \
		--bindir=$(TARGET_ROOT)/sbin \
		--sbindir=$(TARGET_ROOT)/sbin \
		--disable-lastlog \
		--disable-wtmp \
		--host=$(TARGET) )
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_configured

#
# compile dropbear
#

$(STAGEFILES_DIR)/.dropbear_compiled: $(STAGEFILES_DIR)/.dropbear_configured
	$(UCLIBC_ENV) $(MAKE) -C $(DROPBEAR_DIR) \
	  PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 all
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_compiled

#
# install dropbear
#

$(STAGEFILES_DIR)/.dropbear_installed: $(STAGEFILES_DIR)/.dropbear_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(DROPBEAR_DIR) bindir=$(TARGET_ROOT)/sbin \
	  PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 install
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_installed

#
# generate root file list for dropbear
#

$(FILELIST_DIR)/dropbear.lst: $(STAGEFILES_DIR)/.dropbear_installed
	$(TOUCH) $(FILELIST_DIR)/dropbear.lst

.PHONY: clean-dropbear distclean-dropbear

clean-dropbear:
	-$(RM) -rf $(DROPBEAR_DIR)

distclean-dropbear:
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_configured
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DROPBEAR_DLFILE)
endif
