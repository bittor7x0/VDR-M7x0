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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
PCRE_DEPS = $(BASE_BUILD_STAGEFILE)

PCRE_VERSION := 8.39
PCRE_PATCHES_DIR := $(PATCHES_DIR)/pcre/$(PCRE_VERSION)

PCRE_FILE := pcre-$(PCRE_VERSION).tar.bz2
PCRE_DLFILE := $(DOWNLOAD_DIR)/$(PCRE_FILE)
PCRE_URL := http://downloads.sourceforge.net/pcre/$(PCRE_FILE)
PCRE_DIR := $(BUILD_DIR)/pcre-$(PCRE_VERSION)

PCRE_INSTALLED = $(STAGEFILES_DIR)/.pcre_installed

PACKS_RULES_$(CONFIG_PCRE) += $(PCRE_INSTALLED)

FILE_LISTS_$(CONFIG_PCRE) += pcre.lst
FILE_LISTS_$(CONFIG_PCRE_CPP) += pcre-cpp.lst

CLEAN_RULES += clean-pcre
DISTCLEAN_RULES += distclean-pcre

#
# download pcre
#

$(PCRE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PCRE_DLFILE) ] ; then \
	$(WGET) $(PCRE_URL) -O $(PCRE_DLFILE) ; \
	fi );
	$(TOUCH) $(PCRE_DLFILE)

#
# unpack pcre
#

$(STAGEFILES_DIR)/.pcre_unpacked: $(PCRE_DLFILE) \
                                           $(wildcard $(PCRE_PATCHES_DIR)/*.patch) \
                                           $$(PCRE_DEPS)
	-$(RM) -rf $(PCRE_DIR)
	$(BZCAT) $(PCRE_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.pcre_unpacked

#
# patch pcre
#

$(STAGEFILES_DIR)/.pcre_patched: $(STAGEFILES_DIR)/.pcre_unpacked
	$(call patch_package, $(PCRE_DIR), $(PCRE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.pcre_patched

#
# configure pcre
#

$(STAGEFILES_DIR)/.pcre_configured: $(STAGEFILES_DIR)/.pcre_patched
	($(CD) $(PCRE_DIR) ; $(UCLIBC_ENV_LTO_GC_LOOPS) \
		$(PCRE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-jit \
			$(if $(CONFIG_PCRE_CPP),--enable-cpp,--disable-cpp))
	$(TOUCH) $(STAGEFILES_DIR)/.pcre_configured

#
# compile pcre
#

$(STAGEFILES_DIR)/.pcre_compiled: $(STAGEFILES_DIR)/.pcre_configured
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(PCRE_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.pcre_compiled

#
# install pcre
#

$(STAGEFILES_DIR)/.pcre_installed: $(STAGEFILES_DIR)/.pcre_compiled
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(PCRE_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.pcre_installed


$(FILELIST_DIR)/pcre.lst: $(STAGEFILES_DIR)/.pcre_installed
	$(TOUCH) $(FILELIST_DIR)/pcre.lst

$(FILELIST_DIR)/pcre-cpp.lst: $(STAGEFILES_DIR)/.pcre_installed
	$(TOUCH) $(FILELIST_DIR)/pcre-cpp.lst


.PHONY: clean-pcre distclean-pcre

clean-pcre:
	-$(RM) -rf $(PCRE_DIR)

#
# clean everthing else
#

distclean-pcre:
	-$(RM) -f $(STAGEFILES_DIR)/.pcre_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.pcre_patched
	-$(RM) -f $(STAGEFILES_DIR)/.pcre_configured
	-$(RM) -f $(STAGEFILES_DIR)/.pcre_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.pcre_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PCRE_DLFILE)
endif
