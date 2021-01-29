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
UCLIBC++_DEPS = $(BASE_BUILD_STAGEFILE)

UCLIBC++_VERSION := 0.2.4
UCLIBC++_PATCHES_DIR := $(PATCHES_DIR)/uClibc++/$(UCLIBC++_VERSION)
UCLIBC++_CONFIG := $(CONFIGS_DIR)/uClibc++/$(UCLIBC++_VERSION)/uclibc++.config

UCLIBC++_FILE := uClibc++-$(UCLIBC++_VERSION).tar.bz2
UCLIBC++_DLFILE := $(DOWNLOAD_DIR)/$(UCLIBC++_FILE)
UCLIBC++_URL := http://cxx.uclibc.org/src/$(UCLIBC++_FILE)
UCLIBC++_DIR := $(BUILD_DIR)/uClibc++-$(UCLIBC++_VERSION)

UCLIBC++_INSTALLED = $(STAGEFILES_DIR)/.uclibc++_installed

PACKS_RULES_$(CONFIG_UCLIBC++) += $(UCLIBC++_INSTALLED)
FILE_LISTS_$(CONFIG_UCLIBC++) += uclibc++.lst

CLEAN_RULES += clean-uclibc++
DISTCLEAN_RULES += distclean-uclibc++

#
# download uclibc++
#

$(UCLIBC++_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UCLIBC++_DLFILE) ] ; then \
	$(WGET) $(UCLIBC++_URL) -O $(UCLIBC++_DLFILE) ; \
	fi );
	$(TOUCH) $(UCLIBC++_DLFILE)

#
# unpack uclibc++
#

$(STAGEFILES_DIR)/.uclibc++_unpacked: $(UCLIBC++_DLFILE) \
                                           $(wildcard $(UCLIBC++_PATCHES_DIR)/*.patch) \
                                           $(UCLIBC++_CONFIG) \
                                           $$(UCLIBC++_DEPS)
	-$(RM) -rf $(UCLIBC++_DIR)
	$(BZCAT) $(UCLIBC++_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc++_unpacked

#
# patch uclibc++
#

$(STAGEFILES_DIR)/.uclibc++_patched: $(STAGEFILES_DIR)/.uclibc++_unpacked
	$(call patch_package, $(UCLIBC++_DIR), $(UCLIBC++_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc++_patched

#
# configure uclibc++
#

$(STAGEFILES_DIR)/.uclibc++_configured: $(STAGEFILES_DIR)/.uclibc++_patched
	$(CP) $(UCLIBC++_CONFIG) $(UCLIBC++_DIR)/.config
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc++_configured

#
# compile uclibc++ (check_as_needed overrides dependency on libgcc_s)
#

$(STAGEFILES_DIR)/.uclibc++_compiled: $(STAGEFILES_DIR)/.uclibc++_configured
	$(UCLIBC_ENV) $(MAKE) -C $(UCLIBC++_DIR) \
		TOPDIR="$(UCLIBC++_DIR)/" \
		CPU_CFLAGS="$(UCLIBC_CFLAGS)" \
		CROSS="$(UCLIBC_TARGET)-" \
		CP="$(CP)" \
		GEN_LIBS="-lc $(LIBGCC_S)" \
		check_as_needed=
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc++_compiled

#
# install uclibc++
#

$(STAGEFILES_DIR)/.uclibc++_installed: $(STAGEFILES_DIR)/.uclibc++_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(UCLIBC++_DIR) \
		TOPDIR="$(UCLIBC++_DIR)/" \
		CPU_CFLAGS="$(UCLIBC_CFLAGS)" \
		CROSS="$(UCLIBC_TARGET)-" \
		CP="$(CP)" \
		GEN_LIBS="-lc $(LIBGCC_S)" \
		check_as_needed= \
		PREFIX="$(TARGET_ROOT)" \
		install
	$(MV) $(TARGET_ROOT)/bin/g++-uc $(PREFIX_BIN)/$(UCLIBC++_CXX)
	$(SED) -i -e  "s,-I\/usr\/include\/uClibc++\/,-I$(TARGET_ROOT)\/usr\/include\/uClibc++\/,g" $(PREFIX_BIN)/$(UCLIBC++_CXX)
	$(SED) -i -e  "s,-L\/lib\/,-L$(TARGET_ROOT)\/lib\/,g" $(PREFIX_BIN)/$(UCLIBC++_CXX)
# add another wrapper which links against both uClibc++ and libstdc++ 
	$(CP) $(PREFIX_BIN)/$(UCLIBC++_CXX) $(PREFIX_BIN)/$(UCLIBC++_CXX_STD)
	$(SED) -i -e  "s,^WRAPPER_INCLUDEDIR=.*,,g" $(PREFIX_BIN)/$(UCLIBC++_CXX_STD)
	$(SED) -i -e  "s,-luClibc++,-Wl\,-Bdynamic\,-luClibc++\,-Bstatic\,-lstdc++\,-Bdynamic,g" $(PREFIX_BIN)/$(UCLIBC++_CXX_STD)
	$(SED) -i -e  "s,-nostdinc++,,g" $(PREFIX_BIN)/$(UCLIBC++_CXX_STD)
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc++_installed

$(FILELIST_DIR)/uclibc++.lst: $(STAGEFILES_DIR)/.uclibc++_installed
	$(TOUCH) $(FILELIST_DIR)/uclibc++.lst

.PHONY: clean-uclibc++ distclean-uclibc++

clean-uclibc++:
	-$(RM) -rf $(UCLIBC++_DIR)

#
# clean everthing else
#

distclean-uclibc++:
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc++_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc++_patched
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc++_configured
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc++_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc++_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UCLIBC++_DLFILE)
endif
