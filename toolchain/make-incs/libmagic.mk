# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2019 VDR-NG-EM Project
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

ifeq ($(CONFIG_LIBMAGIC),y)
ifneq ($(CONFIG_ZLIB),y)
   $(error dependency error: libmagic needs zlib enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBMAGIC_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

LIBMAGIC_VERSION := 5.04
LIBMAGIC_PATCHES_DIR := $(PATCHES_DIR)/libmagic/$(LIBMAGIC_VERSION)

LIBMAGIC_FILE := file-$(LIBMAGIC_VERSION).tar.gz
LIBMAGIC_DLFILE := $(DOWNLOAD_DIR)/$(LIBMAGIC_FILE)
LIBMAGIC_URL := ftp://ftp.astron.com/pub/file/$(LIBMAGIC_FILE)
LIBMAGIC_DIR := $(BUILD_DIR)/file-$(LIBMAGIC_VERSION)

LIBMAGIC_INSTALLED = $(STAGEFILES_DIR)/.libmagic_installed

PACKS_RULES_$(CONFIG_LIBMAGIC) += $(LIBMAGIC_INSTALLED)
FILE_LISTS_$(CONFIG_LIBMAGIC) += libmagic.lst

CLEAN_RULES += clean-libmagic
DISTCLEAN_RULES += distclean-libmagic

#
# download libmagic
#

$(LIBMAGIC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBMAGIC_DLFILE) ] ; then \
	$(WGET) $(LIBMAGIC_URL) -O $(LIBMAGIC_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBMAGIC_DLFILE)

#
# unpack libmagic
#

$(STAGEFILES_DIR)/.libmagic_unpacked: $(LIBMAGIC_DLFILE) \
                                           $(wildcard $(LIBMAGIC_PATCHES_DIR)/*.patch) \
                                           $$(LIBMAGIC_DEPS)
	-$(RM) -rf $(LIBMAGIC_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBMAGIC_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libmagic_unpacked

#
# patch libmagic
#

$(STAGEFILES_DIR)/.libmagic_patched: $(STAGEFILES_DIR)/.libmagic_unpacked
	$(call patch_package, $(LIBMAGIC_DIR), $(LIBMAGIC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libmagic_patched

#
# configure libmagic
#

$(STAGEFILES_DIR)/.libmagic_configured: $(STAGEFILES_DIR)/.libmagic_patched
	($(CD) $(LIBMAGIC_DIR) ; $(UCLIBC_ENV) \
		$(LIBMAGIC_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libmagic_configured

#
# compile libmagic
#

$(STAGEFILES_DIR)/.libmagic_compiled: $(STAGEFILES_DIR)/.libmagic_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBMAGIC_DIR)/src
	$(TOUCH) $(STAGEFILES_DIR)/.libmagic_compiled

#
# install libmagic
#

$(STAGEFILES_DIR)/.libmagic_installed: $(STAGEFILES_DIR)/.libmagic_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBMAGIC_DIR)/src install
	( $(CD) $(LIBMAGIC_DIR)/magic/Magdir; \
		for f in `ls`; do \
			$(CAT) $${f}; \
		done \
	) > $(LIBMAGIC_DIR)/magic/magic
	$(MKDIR) -p $(TARGET_ROOT)/usr/share/file
	$(CP) -f $(LIBMAGIC_DIR)/magic/magic $(TARGET_ROOT)/usr/share/file/magic
	$(TOUCH) $(STAGEFILES_DIR)/.libmagic_installed


$(FILELIST_DIR)/libmagic.lst: $(STAGEFILES_DIR)/.libmagic_installed
	$(TOUCH) $(FILELIST_DIR)/libmagic.lst

.PHONY: clean-libmagic distclean-libmagic

clean-libmagic:
	-$(RM) -rf $(LIBMAGIC_DIR)

#
# clean everthing else
#

distclean-libmagic:
	-$(RM) -f $(STAGEFILES_DIR)/.libmagic_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libmagic_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libmagic_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libmagic_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libmagic_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBMAGIC_DLFILE)
endif
