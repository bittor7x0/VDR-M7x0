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

DOSFSTOOLS_DEPS = $(BASE_BUILD_STAGEFILE)

DOSFSTOOLS_VERSION := 2.11
DOSFSTOOLS_PATCHES_DIR := $(PATCHES_DIR)/dosfstools/$(DOSFSTOOLS_VERSION)

DOSFSTOOLS_FILE := dosfstools-$(DOSFSTOOLS_VERSION).src.tar.gz
DOSFSTOOLS_DLFILE := $(DOWNLOAD_DIR)/$(DOSFSTOOLS_FILE)
DOSFSTOOLS_URL := https://launchpad.net/dosfstools/main/$(DOSFSTOOLS_VERSION).src/+download/$(DOSFSTOOLS_FILE)
DOSFSTOOLS_DIR := $(BUILD_DIR)/dosfstools-$(DOSFSTOOLS_VERSION)

DOSFSTOOLS_INSTALLED = $(STAGEFILES_DIR)/.dosfstools_installed

PACKS_RULES_$(CONFIG_DOSFSTOOLS) += $(DOSFSTOOLS_INSTALLED)
FILE_LISTS_$(CONFIG_DOSFSTOOLS) += dosfstools.lst

CLEAN_RULES += clean-dosfstools
DISTCLEAN_RULES += distclean-dosfstools

#
# download dosfstools
#

$(DOSFSTOOLS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DOSFSTOOLS_DLFILE) ] ; then \
	$(WGET) $(DOSFSTOOLS_URL) -O $(DOSFSTOOLS_DLFILE) ; \
	fi );
	$(TOUCH) $(DOSFSTOOLS_DLFILE)

#
# unpack dosfstools
#

$(STAGEFILES_DIR)/.dosfstools_unpacked: $(DOSFSTOOLS_DLFILE) \
                                      $(wildcard $(DOSFSTOOLS_PATCHES_DIR)/*.patch) \
                                      $$(DOSFSTOOLS_DEPS)
	-$(RM) -rf $(DOSFSTOOLS_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(DOSFSTOOLS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.dosfstools_unpacked

#
# patch dosfstools
#

$(STAGEFILES_DIR)/.dosfstools_patched: $(STAGEFILES_DIR)/.dosfstools_unpacked 
	$(call patch_package, $(DOSFSTOOLS_DIR), $(DOSFSTOOLS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.dosfstools_patched


#
# compile dosfstools
#

$(STAGEFILES_DIR)/.dosfstools_compiled: $(STAGEFILES_DIR)/.dosfstools_patched
	$(UCLIBC_ENV_SIZE) $(MAKE) -C $(DOSFSTOOLS_DIR) $(UCLIBC_ENV_SIZE) \
		CFLAGS="$(UCLIBC_CFLAGS_SIZE) -D_FILE_OFFSET_BITS=64" \
		PREFIX=$(TARGET_ROOT) all
	$(TOUCH) $(STAGEFILES_DIR)/.dosfstools_compiled

#
# install dosfstools
#

$(STAGEFILES_DIR)/.dosfstools_installed: $(STAGEFILES_DIR)/.dosfstools_compiled
	$(UCLIBC_ENV_SIZE) $(MAKE) -C $(DOSFSTOOLS_DIR) $(UCLIBC_ENV_SIZE) \
		CFLAGS="$(UCLIBC_CFLAGS_SIZE) -D_FILE_OFFSET_BITS=64" \
		PREFIX=$(TARGET_ROOT) install
	$(TOUCH) $(STAGEFILES_DIR)/.dosfstools_installed


$(FILELIST_DIR)/dosfstools.lst: $(STAGEFILES_DIR)/.dosfstools_installed
	$(TOUCH) $(FILELIST_DIR)/dosfstools.lst

.PHONY: clean-dosfstools distclean-dosfstools

clean-dosfstools:
	-$(RM) -rf $(DOSFSTOOLS_BUILD_DIR)
	-$(RM) -rf $(DOSFSTOOLS_DIR)

distclean-dosfstools:
	-$(RM) -f $(STAGEFILES_DIR)/.dosfstools_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dosfstools_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dosfstools_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dosfstools_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DOSFSTOOLS_DLFILE)
endif
