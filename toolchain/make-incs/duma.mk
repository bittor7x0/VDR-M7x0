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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
DUMA_DEPS = $(BASE_BUILD_STAGEFILE)

DUMA_VERSION := 2_5_15
DUMA_PATCHES_DIR := $(PATCHES_DIR)/duma/$(DUMA_VERSION)

DUMA_FILE := duma-$(DUMA_VERSION).tar.gz
DUMA_DLFILE := $(DOWNLOAD_DIR)/$(DUMA_FILE)
DUMA_URL := http://sourceforge.net/projects/duma/files/duma/2.5.15/duma_2_5_15.zip/download
DUMA_DIR := $(BUILD_DIR)/duma_$(DUMA_VERSION)

DUMA_INSTALLED = $(STAGEFILES_DIR)/.duma_installed

PACKS_RULES_$(CONFIG_DUMA) += $(DUMA_INSTALLED)
FILE_LISTS_$(CONFIG_DUMA) += duma.lst

CLEAN_RULES += clean-duma
DISTCLEAN_RULES += distclean-duma

#
# download duma
#

$(DUMA_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DUMA_DLFILE) ] ; then \
	$(WGET) $(DUMA_URL) -O $(DUMA_DLFILE) ; \
	fi );
	$(TOUCH) $(DUMA_DLFILE)

#
# unpack duma
#

$(STAGEFILES_DIR)/.duma_unpacked: $(DUMA_DLFILE) \
                                           $(wildcard $(DUMA_PATCHES_DIR)/*.patch) \
                                           $$(DUMA_DEPS)
	-$(RM) -rf $(DUMA_DIR)
	(cd $(BUILD_DIR) ; $(UNZIP) -x $(DUMA_DLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.duma_unpacked

#
# patch duma
#

$(STAGEFILES_DIR)/.duma_patched: $(STAGEFILES_DIR)/.duma_unpacked
	$(call patch_package, $(DUMA_DIR), $(DUMA_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.duma_patched

#
# compile duma
#

$(STAGEFILES_DIR)/.duma_compiled: $(STAGEFILES_DIR)/.duma_patched
	$(UCLIBC_ENV) HOSTCC=$(CC) $(MAKE) -C $(DUMA_DIR) libduma.a libduma.so.0.0.0
	$(TOUCH) $(STAGEFILES_DIR)/.duma_compiled

#
# install duma
#

$(STAGEFILES_DIR)/.duma_installed: $(STAGEFILES_DIR)/.duma_compiled
	$(CP) $(DUMA_DIR)/libduma.so.0.0.0 $(TARGET_ROOT)/usr/lib/libduma.so.0.0.0
	$(CP) $(DUMA_DIR)/duma.sh $(TARGET_ROOT)/usr/bin/duma
	$(TOUCH) $(STAGEFILES_DIR)/.duma_installed


$(FILELIST_DIR)/duma.lst: $(STAGEFILES_DIR)/.duma_installed
	$(TOUCH) $(FILELIST_DIR)/duma.lst

.PHONY: clean-duma distclean-duma

clean-duma:
	-$(RM) -rf $(DUMA_DIR)

#
# clean everthing else
#

distclean-duma:
	-$(RM) -f $(STAGEFILES_DIR)/.duma_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.duma_patched
	-$(RM) -f $(STAGEFILES_DIR)/.duma_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.duma_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DUMA_DLFILE)
endif
