# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 VDR-NG-EM Project
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
LIBICONV_DEPS = $(BASE_BUILD_STAGEFILE)

LIBICONV_VERSION := 1.11
LIBICONV_PATCHES_DIR := $(PATCHES_DIR)/libiconv/$(LIBICONV_VERSION)

LIBICONV_FILE := libiconv-$(LIBICONV_VERSION).tar.gz
LIBICONV_DLFILE := $(DOWNLOAD_DIR)/$(LIBICONV_FILE)
LIBICONV_URL := http://ftp.gnu.org/pub/gnu/libiconv/$(LIBICONV_FILE)
LIBICONV_DIR := $(BUILD_DIR)/libiconv-$(LIBICONV_VERSION)

LIBICONV_INSTALLED = $(STAGEFILES_DIR)/.libiconv_installed

PACKS_RULES_$(CONFIG_LIBICONV) += $(LIBICONV_INSTALLED)
FILE_LISTS_$(CONFIG_LIBICONV) += libiconv.lst

CLEAN_RULES += clean-libiconv
DISTCLEAN_RULES += distclean-libiconv

#
# download libiconv
#

$(LIBICONV_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBICONV_DLFILE) ] ; then \
	$(WGET) $(LIBICONV_URL) -O $(LIBICONV_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBICONV_DLFILE)

#
# unpack libiconv
#

$(STAGEFILES_DIR)/.libiconv_unpacked: $(LIBICONV_DLFILE) \
                                           $(wildcard $(LIBICONV_PATCHES_DIR)/*.patch) \
                                           $$(LIBICONV_DEPS)
	-$(RM) -rf $(LIBICONV_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBICONV_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libiconv_unpacked

#
# patch libiconv
#

$(STAGEFILES_DIR)/.libiconv_patched: $(STAGEFILES_DIR)/.libiconv_unpacked
	$(call patch_package, $(LIBICONV_DIR), $(LIBICONV_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libiconv_patched

#
# configure libiconv
#

$(STAGEFILES_DIR)/.libiconv_configured: $(STAGEFILES_DIR)/.libiconv_patched
	($(CD) $(LIBICONV_DIR) ; $(UCLIBC_ENV) \
		$(LIBICONV_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr/lib/libiconv \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--enable-relocatable \
			--without-libintl-prefix)
	$(TOUCH) $(STAGEFILES_DIR)/.libiconv_configured

#
# compile libiconv
#

$(STAGEFILES_DIR)/.libiconv_compiled: $(STAGEFILES_DIR)/.libiconv_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBICONV_DIR) \
		CFLAGS="$(UCLIBC_CFLAGS) -fPIC -DUSE_DOS"
	$(TOUCH) $(STAGEFILES_DIR)/.libiconv_compiled

#
# install libiconv
#

$(STAGEFILES_DIR)/.libiconv_installed: $(STAGEFILES_DIR)/.libiconv_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBICONV_DIR) install
	$(CP) $(TARGET_ROOT)/usr/lib/libiconv/lib/libcharset.so* $(TARGET_ROOT)/usr/lib/
	$(CP) $(TARGET_ROOT)/usr/lib/libiconv/lib/libiconv.so* $(TARGET_ROOT)/usr/lib/
	$(TOUCH) $(STAGEFILES_DIR)/.libiconv_installed


$(FILELIST_DIR)/libiconv.lst: $(STAGEFILES_DIR)/.libiconv_installed
	$(TOUCH) $(FILELIST_DIR)/libiconv.lst

.PHONY: clean-libiconv distclean-libiconv

clean-libiconv:
	-$(RM) -rf $(LIBICONV_DIR)

#
# clean everthing else
#

distclean-libiconv:
	-$(RM) -f $(STAGEFILES_DIR)/.libiconv_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libiconv_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libiconv_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libiconv_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libiconv_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBICONV_DLFILE)
endif
