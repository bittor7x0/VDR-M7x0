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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
EXPAT_DEPS = $(BASE_BUILD_STAGEFILE)

EXPAT_VERSION := 2.2.9
EXPAT_PATCHES_DIR := $(PATCHES_DIR)/expat/$(EXPAT_VERSION)

EXPAT_FILE := expat-$(EXPAT_VERSION).tar.xz
EXPAT_DLFILE := $(DOWNLOAD_DIR)/$(EXPAT_FILE)
EXPAT_URL := http://downloads.sourceforge.net/expat/$(EXPAT_FILE)
EXPAT_DIR := $(BUILD_DIR)/expat-$(EXPAT_VERSION)

EXPAT_INSTALLED = $(STAGEFILES_DIR)/.expat_installed

PACKS_RULES_$(CONFIG_EXPAT) += $(EXPAT_INSTALLED)
FILE_LISTS_$(CONFIG_EXPAT) += expat.lst

CLEAN_RULES += clean-expat
DISTCLEAN_RULES += distclean-expat

#
# download expat
#

$(EXPAT_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(EXPAT_DLFILE) ] ; then \
	$(WGET) $(EXPAT_URL) -O $(EXPAT_DLFILE) ; \
	fi );
	$(TOUCH) $(EXPAT_DLFILE)

#
# unpack expat
#

$(STAGEFILES_DIR)/.expat_unpacked: $(EXPAT_DLFILE) \
                                           $(wildcard $(EXPAT_PATCHES_DIR)/*.patch) \
                                           $$(EXPAT_DEPS)
	-$(RM) -rf $(EXPAT_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(EXPAT_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.expat_unpacked

#
# patch expat
#

$(STAGEFILES_DIR)/.expat_patched: $(STAGEFILES_DIR)/.expat_unpacked
	$(call patch_package, $(EXPAT_DIR), $(EXPAT_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.expat_patched

#
# configure expat
#

$(STAGEFILES_DIR)/.expat_configured: $(STAGEFILES_DIR)/.expat_patched
	($(CD) $(EXPAT_DIR) ; $(UCLIBC_ENV_LTO_GC_LOOPS) \
		$(EXPAT_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--without-xmlwf \
			--without-examples \
			--without-tests \
			--without-docbook \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.expat_configured

#
# compile expat
#

$(STAGEFILES_DIR)/.expat_compiled: $(STAGEFILES_DIR)/.expat_configured
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(EXPAT_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.expat_compiled

#
# install expat
#

$(STAGEFILES_DIR)/.expat_installed: $(STAGEFILES_DIR)/.expat_compiled
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(EXPAT_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.expat_installed


$(FILELIST_DIR)/expat.lst: $(STAGEFILES_DIR)/.expat_installed
	$(TOUCH) $(FILELIST_DIR)/expat.lst

.PHONY: clean-expat distclean-expat

clean-expat:
	-$(RM) -rf $(EXPAT_DIR)

#
# clean everthing else
#

distclean-expat:
	-$(RM) -f $(STAGEFILES_DIR)/.expat_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.expat_patched
	-$(RM) -f $(STAGEFILES_DIR)/.expat_configured
	-$(RM) -f $(STAGEFILES_DIR)/.expat_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.expat_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(EXPAT_DLFILE)
endif
