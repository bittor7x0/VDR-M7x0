# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 VDR-NG-EM Project
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

ifeq ($(CONFIG_TAGLIB),y)
ifneq ($(CONFIG_ZLIB),y)
   $(error dependency error: taglib needs zlib enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
TAGLIB_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

TAGLIB_VERSION := 1.6.3
TAGLIB_PATCHES_DIR := $(PATCHES_DIR)/taglib/$(TAGLIB_VERSION)

TAGLIB_FILE := taglib-$(TAGLIB_VERSION).tar.gz
TAGLIB_DLFILE := $(DOWNLOAD_DIR)/$(TAGLIB_FILE)
TAGLIB_URL := http://developer.kde.org/~wheeler/files/src/$(TAGLIB_FILE)
TAGLIB_DIR := $(BUILD_DIR)/taglib-$(TAGLIB_VERSION)

TAGLIB_INSTALLED = $(STAGEFILES_DIR)/.taglib_installed

PACKS_RULES_$(CONFIG_TAGLIB) += $(TAGLIB_INSTALLED)
FILE_LISTS_$(CONFIG_TAGLIB) += taglib.lst

CLEAN_RULES += clean-taglib
DISTCLEAN_RULES += distclean-taglib

#
# download taglib
#

$(TAGLIB_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(TAGLIB_DLFILE) ] ; then \
	$(WGET) $(TAGLIB_URL) -O $(TAGLIB_DLFILE) ; \
	fi );
	$(TOUCH) $(TAGLIB_DLFILE)

#
# unpack taglib
#

$(STAGEFILES_DIR)/.taglib_unpacked: $(TAGLIB_DLFILE) \
                                           $(wildcard $(TAGLIB_PATCHES_DIR)/*.patch) \
                                           $$(TAGLIB_DEPS)
	-$(RM) -rf $(TAGLIB_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(TAGLIB_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.taglib_unpacked

#
# patch taglib
#

$(STAGEFILES_DIR)/.taglib_patched: $(STAGEFILES_DIR)/.taglib_unpacked
	$(call patch_package, $(TAGLIB_DIR), $(TAGLIB_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.taglib_patched

#
# configure taglib
#

$(STAGEFILES_DIR)/.taglib_configured: $(STAGEFILES_DIR)/.taglib_patched
	($(CD) $(TAGLIB_DIR) ; $(UCLIBC_ENV) \
		$(TAGLIB_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.taglib_configured

#
# compile taglib
#

$(STAGEFILES_DIR)/.taglib_compiled: $(STAGEFILES_DIR)/.taglib_configured
	$(UCLIBC_ENV) $(MAKE) -C $(TAGLIB_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.taglib_compiled

#
# install taglib
#

$(STAGEFILES_DIR)/.taglib_installed: $(STAGEFILES_DIR)/.taglib_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(TAGLIB_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.taglib_installed


$(FILELIST_DIR)/taglib.lst: $(STAGEFILES_DIR)/.taglib_installed
	$(TOUCH) $(FILELIST_DIR)/taglib.lst

.PHONY: clean-taglib distclean-taglib

clean-taglib:
	-$(RM) -rf $(TAGLIB_DIR)

#
# clean everthing else
#

distclean-taglib:
	-$(RM) -f $(STAGEFILES_DIR)/.taglib_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.taglib_patched
	-$(RM) -f $(STAGEFILES_DIR)/.taglib_configured
	-$(RM) -f $(STAGEFILES_DIR)/.taglib_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.taglib_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(TAGLIB_DLFILE)
endif
