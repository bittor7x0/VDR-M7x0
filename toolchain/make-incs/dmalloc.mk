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
DMALLOC_DEPS = $(BASE_BUILD_STAGEFILE)

DMALLOC_VERSION := 5.5.2
DMALLOC_PATCHES_DIR := $(PATCHES_DIR)/dmalloc/$(DMALLOC_VERSION)

DMALLOC_FILE := dmalloc-$(DMALLOC_VERSION).tgz
DMALLOC_DLFILE := $(DOWNLOAD_DIR)/$(DMALLOC_FILE)
DMALLOC_URL := http://dmalloc.com/releases/$(DMALLOC_FILE)
DMALLOC_DIR := $(BUILD_DIR)/dmalloc-$(DMALLOC_VERSION)

DMALLOC_INSTALLED = $(STAGEFILES_DIR)/.dmalloc_installed

PACKS_RULES_$(CONFIG_DMALLOC) += $(DMALLOC_INSTALLED)
FILE_LISTS_$(CONFIG_DMALLOC) += dmalloc.lst

CLEAN_RULES += clean-dmalloc
DISTCLEAN_RULES += distclean-dmalloc

#
# download dmalloc
#

$(DMALLOC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DMALLOC_DLFILE) ] ; then \
	$(WGET) $(DMALLOC_URL) -O $(DMALLOC_DLFILE) ; \
	fi );
	$(TOUCH) $(DMALLOC_DLFILE)

#
# unpack dmalloc
#

$(STAGEFILES_DIR)/.dmalloc_unpacked: $(DMALLOC_DLFILE) \
                                           $(wildcard $(DMALLOC_PATCHES_DIR)/*.patch) \
                                           $$(DMALLOC_DEPS)
	-$(RM) -rf $(DMALLOC_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(DMALLOC_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.dmalloc_unpacked

#
# patch dmalloc
#

$(STAGEFILES_DIR)/.dmalloc_patched: $(STAGEFILES_DIR)/.dmalloc_unpacked
	$(call patch_package, $(DMALLOC_DIR), $(DMALLOC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.dmalloc_patched

#
# configure dmalloc
#

$(STAGEFILES_DIR)/.dmalloc_configured: $(STAGEFILES_DIR)/.dmalloc_patched
	($(CD) $(DMALLOC_DIR) ; $(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" \
		$(DMALLOC_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-cxx \
			--with-pagesize=12)
	$(TOUCH) $(STAGEFILES_DIR)/.dmalloc_configured

#
# compile dmalloc
#

$(STAGEFILES_DIR)/.dmalloc_compiled: $(STAGEFILES_DIR)/.dmalloc_configured
	$(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" $(MAKE) \
		-C $(DMALLOC_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dmalloc_compiled

#
# install dmalloc
#

$(STAGEFILES_DIR)/.dmalloc_installed: $(STAGEFILES_DIR)/.dmalloc_compiled
	$(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" $(MAKE) \
		-C $(DMALLOC_DIR) install installsl
	$(TOUCH) $(STAGEFILES_DIR)/.dmalloc_installed


$(FILELIST_DIR)/dmalloc.lst: $(STAGEFILES_DIR)/.dmalloc_installed
	$(TOUCH) $(FILELIST_DIR)/dmalloc.lst

.PHONY: clean-dmalloc distclean-dmalloc

clean-dmalloc:
	-$(RM) -rf $(DMALLOC_DIR)

#
# clean everthing else
#

distclean-dmalloc:
	-$(RM) -f $(STAGEFILES_DIR)/.dmalloc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dmalloc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dmalloc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.dmalloc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dmalloc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DMALLOC_DLFILE)
endif
