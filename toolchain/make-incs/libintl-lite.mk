# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2012-2021 VDR-NG-EM Project
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
LIBINTL-LITE_DEPS = $(BASE_BUILD_STAGEFILE)

LIBINTL-LITE_VERSION := 4806f5c11f18ba77025cde2eacabca348f3172f3
LIBINTL-LITE_PATCHES_DIR := $(PATCHES_DIR)/libintl-lite/snapshot

LIBINTL-LITE_FILE := libintl-lite-$(LIBINTL-LITE_VERSION).zip
LIBINTL-LITE_DLFILE := $(DOWNLOAD_DIR)/$(LIBINTL-LITE_FILE)
LIBINTL-LITE_URL := https://github.com/j-jorge/libintl-lite/archive/$(LIBINTL-LITE_VERSION).zip
LIBINTL-LITE_DIR := $(BUILD_DIR)/libintl-lite-$(LIBINTL-LITE_VERSION)

LIBINTL-LITE_INSTALLED = $(STAGEFILES_DIR)/.libintl-lite_installed

PACKS_RULES_$(CONFIG_LIBINTL-LITE) += $(LIBINTL-LITE_INSTALLED)
FILE_LISTS_$(CONFIG_LIBINTL-LITE) += libintl-lite.lst

CLEAN_RULES += clean-libintl-lite
DISTCLEAN_RULES += distclean-libintl-lite

#
# download libintl-lite
#

$(LIBINTL-LITE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBINTL-LITE_DLFILE) ] ; then \
	$(WGET) $(LIBINTL-LITE_URL) -O $(LIBINTL-LITE_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBINTL-LITE_DLFILE)

#
# unpack libintl-lite
#

$(STAGEFILES_DIR)/.libintl-lite_unpacked: $(LIBINTL-LITE_DLFILE) \
                                           $(wildcard $(LIBINTL-LITE_PATCHES_DIR)/*.patch) \
                                           $$(LIBINTL-LITE_DEPS)
	-$(RM) -rf $(LIBINTL-LITE_DIR)
	(cd $(BUILD_DIR) ; $(UNZIP) -x $(LIBINTL-LITE_DLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.libintl-lite_unpacked

#
# patch libintl-lite
#

$(STAGEFILES_DIR)/.libintl-lite_patched: $(STAGEFILES_DIR)/.libintl-lite_unpacked
	$(call patch_package, $(LIBINTL-LITE_DIR), $(LIBINTL-LITE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libintl-lite_patched

#
# compile libintl-lite
#

$(STAGEFILES_DIR)/.libintl-lite_compiled: $(STAGEFILES_DIR)/.libintl-lite_patched
	$(PREFIX_BIN)/$(UCLIBC_CXX) $(UCLIBC_CXXFLAGS_LTO_GC) -Wall -fPIC -c $(LIBINTL-LITE_DIR)/internal/libintl.cpp -o $(LIBINTL-LITE_DIR)/libintl.o
	$(PREFIX_BIN)/$(UCLIBC_CXX) $(UCLIBC_CXXFLAGS_LTO_GC) -Wall -fPIC -shared -Wl,-soname,libintl.so.1 -o $(LIBINTL-LITE_DIR)/libintl.so.1.0.0 $(LIBINTL-LITE_DIR)/libintl.o
	$(TOUCH) $(STAGEFILES_DIR)/.libintl-lite_compiled

#
# install libintl-lite
#

$(STAGEFILES_DIR)/.libintl-lite_installed: $(STAGEFILES_DIR)/.libintl-lite_compiled
	$(CP) $(LIBINTL-LITE_DIR)/libintl.h $(TARGET_ROOT)/usr/include/libintl.h
	$(CP) $(LIBINTL-LITE_DIR)/libintl.so.1.0.0 $(TARGET_ROOT)/usr/lib/libintl.so.1.0.0
	$(LN) -sf $(TARGET_ROOT)/usr/lib/libintl.so.1.0.0 $(TARGET_ROOT)/usr/lib/libintl.so
	$(LN) -sf $(TARGET_ROOT)/usr/lib/libintl.so.1.0.0 $(TARGET_ROOT)/usr/lib/libintl.so.1
	$(TOUCH) $(STAGEFILES_DIR)/.libintl-lite_installed


$(FILELIST_DIR)/libintl-lite.lst: $(STAGEFILES_DIR)/.libintl-lite_installed
	$(TOUCH) $(FILELIST_DIR)/libintl-lite.lst

.PHONY: clean-libintl-lite distclean-libintl-lite

clean-libintl-lite:
	-$(RM) -rf $(LIBINTL-LITE_DIR)

#
# clean everthing else
#

distclean-libintl-lite:
	-$(RM) -f $(STAGEFILES_DIR)/.libintl-lite_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libintl-lite_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libintl-lite_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libintl-lite_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBINTL-LITE_DLFILE)
endif
