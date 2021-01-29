# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2021 VDR-NG-EM Project
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

ifeq ($(CONFIG_ID3LIB),y)
ifneq ($(CONFIG_ZLIB),y)
   $(error dependency error: id3lib needs zlib enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
ID3LIB_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

ID3LIB_VERSION := 3.8.3
ID3LIB_PATCHES_DIR := $(PATCHES_DIR)/id3lib/$(ID3LIB_VERSION)

ID3LIB_FILE := id3lib-$(ID3LIB_VERSION).tar.gz
ID3LIB_DLFILE := $(DOWNLOAD_DIR)/$(ID3LIB_FILE)
ID3LIB_URL := http://downloads.sourceforge.net/id3lib/$(ID3LIB_FILE)
ID3LIB_DIR := $(BUILD_DIR)/id3lib-$(ID3LIB_VERSION)

ID3LIB_INSTALLED = $(STAGEFILES_DIR)/.id3lib_installed

PACKS_RULES_$(CONFIG_ID3LIB) += $(ID3LIB_INSTALLED)
FILE_LISTS_$(CONFIG_ID3LIB) += id3lib.lst

CLEAN_RULES += clean-id3lib
DISTCLEAN_RULES += distclean-id3lib

#
# download id3lib
#

$(ID3LIB_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(ID3LIB_DLFILE) ] ; then \
	$(WGET) $(ID3LIB_URL) -O $(ID3LIB_DLFILE) ; \
	fi );
	$(TOUCH) $(ID3LIB_DLFILE)

#
# unpack id3lib
#

$(STAGEFILES_DIR)/.id3lib_unpacked: $(ID3LIB_DLFILE) \
                                           $(wildcard $(ID3LIB_PATCHES_DIR)/*.patch) \
                                           $$(ID3LIB_DEPS)
	-$(RM) -rf $(ID3LIB_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(ID3LIB_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.id3lib_unpacked

#
# patch id3lib
#

$(STAGEFILES_DIR)/.id3lib_patched: $(STAGEFILES_DIR)/.id3lib_unpacked
	$(call patch_package, $(ID3LIB_DIR), $(ID3LIB_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.id3lib_patched

#
# configure id3lib
#

$(STAGEFILES_DIR)/.id3lib_configured: $(STAGEFILES_DIR)/.id3lib_patched
	($(CD) $(ID3LIB_DIR) ; $(UCLIBC_ENV) \
		$(ID3LIB_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--disable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.id3lib_configured

#
# compile id3lib
#

$(STAGEFILES_DIR)/.id3lib_compiled: $(STAGEFILES_DIR)/.id3lib_configured
	$(UCLIBC_ENV) $(MAKE) -C $(ID3LIB_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.id3lib_compiled

#
# install id3lib
#

$(STAGEFILES_DIR)/.id3lib_installed: $(STAGEFILES_DIR)/.id3lib_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(ID3LIB_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.id3lib_installed


$(FILELIST_DIR)/id3lib.lst: $(STAGEFILES_DIR)/.id3lib_installed
	$(TOUCH) $(FILELIST_DIR)/id3lib.lst

.PHONY: clean-id3lib distclean-id3lib

clean-id3lib:
	-$(RM) -rf $(ID3LIB_DIR)

#
# clean everthing else
#

distclean-id3lib:
	-$(RM) -f $(STAGEFILES_DIR)/.id3lib_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.id3lib_patched
	-$(RM) -f $(STAGEFILES_DIR)/.id3lib_configured
	-$(RM) -f $(STAGEFILES_DIR)/.id3lib_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.id3lib_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(ID3LIB_DLFILE)
endif
