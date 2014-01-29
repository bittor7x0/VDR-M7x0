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
LIBXML2_DEPS = $(BASE_BUILD_STAGEFILE)

LIBXML2_VERSION := 2.7.7
LIBXML2_PATCHES_DIR := $(PATCHES_DIR)/libxml2/$(LIBXML2_VERSION)

LIBXML2_FILE := libxml2-$(LIBXML2_VERSION).tar.gz
LIBXML2_DLFILE := $(DOWNLOAD_DIR)/$(LIBXML2_FILE)
LIBXML2_URL := http://xmlsoft.org/sources/$(LIBXML2_FILE)
LIBXML2_DIR := $(BUILD_DIR)/libxml2-$(LIBXML2_VERSION)

LIBXML2_INSTALLED = $(STAGEFILES_DIR)/.libxml2_installed

PACKS_RULES_$(CONFIG_LIBXML2) += $(LIBXML2_INSTALLED)
FILE_LISTS_$(CONFIG_LIBXML2) += libxml2.lst

CLEAN_RULES += clean-libxml2
DISTCLEAN_RULES += distclean-libxml2

#
# download libxml2
#

$(LIBXML2_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBXML2_DLFILE) ] ; then \
	$(WGET) $(LIBXML2_URL) -O $(LIBXML2_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBXML2_DLFILE)

#
# unpack libxml2
#

$(STAGEFILES_DIR)/.libxml2_unpacked: $(LIBXML2_DLFILE) \
                                           $(wildcard $(LIBXML2_PATCHES_DIR)/*.patch) \
                                           $$(LIBXML2_DEPS)
	-$(RM) -rf $(LIBXML2_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBXML2_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libxml2_unpacked

#
# patch libxml2
#

$(STAGEFILES_DIR)/.libxml2_patched: $(STAGEFILES_DIR)/.libxml2_unpacked
	$(call patch_package, $(LIBXML2_DIR), $(LIBXML2_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libxml2_patched

#
# configure libxml2
#

$(STAGEFILES_DIR)/.libxml2_configured: $(STAGEFILES_DIR)/.libxml2_patched
	($(CD) $(LIBXML2_DIR) ; $(UCLIBC_ENV) \
		$(LIBXML2_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--with-c14n \
			--without-catalog \
			--without-debug \
			--without-docbook \
			--without-fexceptions \
			--without-history \
			--with-html \
			--without-ftp \
			--without-http \
			--without-iconv \
			--without-iso8859x \
			--without-legacy \
			--without-mem-debug \
			--with-output \
			--without-pattern \
			--without-push \
			--without-python \
			--with-reader \
			--without-readline \
			--without-regexps \
			--without-run-debug \
			--with-sax1 \
			--with-schemas \
			--without-schematron \
			--with-threads \
			--without-thread-alloc \
			--with-tree \
			--with-valid \
			--with-writer \
			--with-xinclude \
			--with-xpath \
			--with-xptr \
			--without-modules \
			$(if $(CONFIG_ZLIB),--with-zlib,--without-zlib) \
			--without-coverage)
	$(TOUCH) $(STAGEFILES_DIR)/.libxml2_configured

#
# compile libxml2
#

$(STAGEFILES_DIR)/.libxml2_compiled: $(STAGEFILES_DIR)/.libxml2_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBXML2_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libxml2_compiled

#
# install libxml2
#

$(STAGEFILES_DIR)/.libxml2_installed: $(STAGEFILES_DIR)/.libxml2_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBXML2_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libxml2_installed


$(FILELIST_DIR)/libxml2.lst: $(STAGEFILES_DIR)/.libxml2_installed
	$(TOUCH) $(FILELIST_DIR)/libxml2.lst

.PHONY: clean-libxml2 distclean-libxml2

clean-libxml2:
	-$(RM) -rf $(LIBXML2_DIR)

#
# clean everthing else
#

distclean-libxml2:
	-$(RM) -f $(STAGEFILES_DIR)/.libxml2_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libxml2_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libxml2_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libxml2_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libxml2_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBXML2_DLFILE)
endif
