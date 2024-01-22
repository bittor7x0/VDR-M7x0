# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2016-2024 VDR-NG-EM Project
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
JANSSON_DEPS = $(BASE_BUILD_STAGEFILE)

JANSSON_VERSION := 2.13.1
JANSSON_PATCHES_DIR := $(PATCHES_DIR)/jansson/$(JANSSON_VERSION)

JANSSON_FILE := jansson-$(JANSSON_VERSION).tar.bz2
JANSSON_DLFILE := $(DOWNLOAD_DIR)/$(JANSSON_FILE)
JANSSON_URL := http://www.digip.org/jansson/releases/$(JANSSON_FILE)
JANSSON_DIR := $(BUILD_DIR)/jansson-$(JANSSON_VERSION)

JANSSON_INSTALLED = $(STAGEFILES_DIR)/.jansson_installed

PACKS_RULES_$(CONFIG_JANSSON) += $(JANSSON_INSTALLED)
FILE_LISTS_$(CONFIG_JANSSON) += jansson.lst

CLEAN_RULES += clean-jansson
DISTCLEAN_RULES += distclean-jansson

#
# download jansson
#

$(JANSSON_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(JANSSON_DLFILE) ] ; then \
	$(WGET) $(JANSSON_URL) -O $(JANSSON_DLFILE) ; \
	fi );
	$(TOUCH) $(JANSSON_DLFILE)

#
# unpack jansson
#

$(STAGEFILES_DIR)/.jansson_unpacked: $(JANSSON_DLFILE) \
                                           $(wildcard $(JANSSON_PATCHES_DIR)/*.patch) \
                                           $$(JANSSON_DEPS)
	-$(RM) -rf $(JANSSON_DIR)
	$(BZCAT) $(JANSSON_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.jansson_unpacked

#
# patch jansson
#

$(STAGEFILES_DIR)/.jansson_patched: $(STAGEFILES_DIR)/.jansson_unpacked
	$(call patch_package, $(JANSSON_DIR), $(JANSSON_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.jansson_patched

#
# configure jansson
#

$(STAGEFILES_DIR)/.jansson_configured: $(STAGEFILES_DIR)/.jansson_patched
	($(CD) $(JANSSON_DIR) ; $(UCLIBC_ENV_LTO) \
		LDFLAGS="$(UCLIBC_LDFLAGS_LTO) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib -lm" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(JANSSON_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.jansson_configured

#
# compile jansson
#

$(STAGEFILES_DIR)/.jansson_compiled: $(STAGEFILES_DIR)/.jansson_configured
	$(MAKE) -C $(JANSSON_DIR) $(UCLIBC_ENV_LTO)
	$(TOUCH) $(STAGEFILES_DIR)/.jansson_compiled

#
# install jansson
#

$(STAGEFILES_DIR)/.jansson_installed: $(STAGEFILES_DIR)/.jansson_compiled
	$(MAKE) -C $(JANSSON_DIR) $(UCLIBC_ENV_LTO) install
	$(TOUCH) $(STAGEFILES_DIR)/.jansson_installed


$(FILELIST_DIR)/jansson.lst: $(STAGEFILES_DIR)/.jansson_installed
	$(TOUCH) $(FILELIST_DIR)/jansson.lst

.PHONY: clean-jansson distclean-jansson

clean-jansson:
	-$(RM) -rf $(JANSSON_DIR)

#
# clean everthing else
#

distclean-jansson:
	-$(RM) -f $(STAGEFILES_DIR)/.jansson_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.jansson_patched
	-$(RM) -f $(STAGEFILES_DIR)/.jansson_configured
	-$(RM) -f $(STAGEFILES_DIR)/.jansson_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.jansson_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(JANSSON_DLFILE)
endif
