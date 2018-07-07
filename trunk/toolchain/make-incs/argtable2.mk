# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2018 VDR-NG-EM Project
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
ARGTABLE2_DEPS = $(BASE_BUILD_STAGEFILE)

ARGTABLE2_VERSION := 2-13
ARGTABLE2_VERSION_POINT := 2.13
ARGTABLE2_PATCHES_DIR := $(PATCHES_DIR)/argtable2/$(ARGTABLE2_VERSION_POINT)

ARGTABLE2_FILE := argtable$(ARGTABLE2_VERSION).tar.gz
ARGTABLE2_DLFILE := $(DOWNLOAD_DIR)/$(ARGTABLE2_FILE)
ARGTABLE2_URL := https://sourceforge.net/projects/argtable/files/argtable/argtable-$(ARGTABLE2_VERSION_POINT)/$(ARGTABLE2_FILE)/download
ARGTABLE2_DIR := $(BUILD_DIR)/argtable$(ARGTABLE2_VERSION)

ARGTABLE2_INSTALLED = $(STAGEFILES_DIR)/.argtable2_installed

PACKS_RULES_$(CONFIG_ARGTABLE2) += $(ARGTABLE2_INSTALLED)
FILE_LISTS_$(CONFIG_ARGTABLE2) += argtable2.lst

CLEAN_RULES += clean-argtable2
DISTCLEAN_RULES += distclean-argtable2

#
# download argtable2
#

$(ARGTABLE2_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(ARGTABLE2_DLFILE) ] ; then \
	$(WGET) $(ARGTABLE2_URL) -O $(ARGTABLE2_DLFILE) ; \
	fi );
	$(TOUCH) $(ARGTABLE2_DLFILE)

#
# unpack argtable2
#

$(STAGEFILES_DIR)/.argtable2_unpacked: $(ARGTABLE2_DLFILE) \
                                           $(wildcard $(ARGTABLE2_PATCHES_DIR)/*.patch) \
                                           $$(ARGTABLE2_DEPS)
	-$(RM) -rf $(ARGTABLE2_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(ARGTABLE2_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.argtable2_unpacked

#
# patch argtable2
#

$(STAGEFILES_DIR)/.argtable2_patched: $(STAGEFILES_DIR)/.argtable2_unpacked
	$(call patch_package, $(ARGTABLE2_DIR), $(ARGTABLE2_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.argtable2_patched

#
# configure argtable2
#

$(STAGEFILES_DIR)/.argtable2_configured: $(STAGEFILES_DIR)/.argtable2_patched
	($(CD) $(ARGTABLE2_DIR) ; $(UCLIBC_ENV_GC) \
		CFLAGS="$(UCLIBC_CFLAGS_GC) -fPIC" \
		$(ARGTABLE2_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.argtable2_configured

#
# compile argtable2
#

$(STAGEFILES_DIR)/.argtable2_compiled: $(STAGEFILES_DIR)/.argtable2_configured
	$(MAKE) -C $(ARGTABLE2_DIR) $(UCLIBC_ENV_GC)
	$(TOUCH) $(STAGEFILES_DIR)/.argtable2_compiled

#
# install argtable2
#

$(STAGEFILES_DIR)/.argtable2_installed: $(STAGEFILES_DIR)/.argtable2_compiled
	$(MAKE) -C $(ARGTABLE2_DIR) $(UCLIBC_ENV_GC) install
	$(TOUCH) $(STAGEFILES_DIR)/.argtable2_installed


$(FILELIST_DIR)/argtable2.lst: $(STAGEFILES_DIR)/.argtable2_installed
	$(TOUCH) $(FILELIST_DIR)/argtable2.lst

.PHONY: clean-argtable2 distclean-argtable2

clean-argtable2:
	-$(RM) -rf $(ARGTABLE2_DIR)

#
# clean everthing else
#

distclean-argtable2:
	-$(RM) -f $(STAGEFILES_DIR)/.argtable2_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.argtable2_patched
	-$(RM) -f $(STAGEFILES_DIR)/.argtable2_configured
	-$(RM) -f $(STAGEFILES_DIR)/.argtable2_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.argtable2_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(ARGTABLE2_DLFILE)
endif
