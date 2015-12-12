# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
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

# http://git.uclibc.org/uClibc/commit/?id=ca1c74d67dd115d059a875150e10b8560a9c35a8
UCLIBC_VERSION := ca1c74d67dd115d059a875150e10b8560a9c35a8
UCLIBC_IS_SNAPSHOT := y

UCLIBC_O7OVERSION_H := o7o-toolchain-version.h
UCLIBC_FILE := uClibc-$(UCLIBC_VERSION).tar.bz2
UCLIBC_DLFILE := $(DOWNLOAD_DIR)/$(UCLIBC_FILE)
UCLIBC_DIR := $(BUILD_DIR)/uClibc-$(UCLIBC_VERSION)

ifeq ($(UCLIBC_IS_SNAPSHOT),y)
UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc/snapshot/uclibc.config
UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc/snapshot
UCLIBC_URL := http://git.uclibc.org/uClibc/snapshot/$(UCLIBC_FILE)
else
UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc/$(UCLIBC_VERSION)/uclibc.config
UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc/$(UCLIBC_VERSION)
UCLIBC_URL := http://www.uclibc.org/downloads/$(UCLIBC_FILE)
endif

UCLIBC_PRE_ALL_GCC = $(UCLIBC_DLFILE) $(wildcard $(UCLIBC_PATCHES_DIR)/*.patch) $(UCLIBC_CONFIG) \
   $(STAGEFILES_DIR)/.uclibc_headers_installed
UCLIBC_INSTALLED = $(STAGEFILES_DIR)/.uclibc_installed


BASE_RULES_y += $(STAGEFILES_DIR)/.uclibc_utils_installed
CLEAN_RULES += clean-uclibc
DISTCLEAN_RULES += distclean-uclibc
FILE_LISTS_y += uclibc.lst

ifeq ($(CONFIG_UCLIBC_WITH_BACKTRACE),y)
FILE_LISTS_y +=  uclibc-backtrace.lst
endif

#
# download uclibc
#

$(UCLIBC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UCLIBC_DLFILE) ] ; then \
	$(WGET) $(UCLIBC_URL) -O $(UCLIBC_DLFILE) ; \
	fi );
	$(TOUCH) $(UCLIBC_DLFILE)

#
# unpack uClibc
#

$(STAGEFILES_DIR)/.uclibc_unpacked: $(UCLIBC_DLFILE) \
                                    $(wildcard $(UCLIBC_PATCHES_DIR)/*.patch) \
                                    $$(LINUX_HEADERS_INSTALLED) \
                                    $(UCLIBC_CONFIG) \
                                    $$(BINUTILS_INSTALLED) \
                                    $(if $(CONFIG_CCACHE),$$(CCACHE_HOSTINSTALLED)) \
                                    $$(SSTRIP_HOSTINSTALLED)
	-$(RM) -rf $(UCLIBC_DIR)
	$(BZCAT) $(UCLIBC_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_unpacked

#
# patch uClibc
#

$(STAGEFILES_DIR)/.uclibc_patched: $(STAGEFILES_DIR)/.uclibc_unpacked
	$(call patch_package, $(UCLIBC_DIR), $(UCLIBC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_patched

#
# configure uClibc
#

$(STAGEFILES_DIR)/.uclibc_configured: $(STAGEFILES_DIR)/.uclibc_patched
	$(SED) -ne 's,^KERNEL_HEADERS=.*,KERNEL_HEADERS=\"$(LINUX_HEADERS_INSTALL_DIR)\",g' \
		-e 'w$(UCLIBC_DIR)/.config' $(UCLIBC_CONFIG)
  # Disable locale in Cygwin
  ifeq ($(HOST_CYGWIN),y)
	$(SED) -i -e 's,^UCLIBC_HAS_LOCALE=y,# UCLIBC_HAS_LOCALE is not set,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^UCLIBC_BUILD_MINIMAL_LOCALE=y,,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^UCLIBC_BUILD_MINIMAL_LOCALES="en_US es_ES de_DE fr_FR",,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^UCLIBC_HAS_XLOCALE=y,,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^# UCLIBC_HAS_GLIBC_DIGIT_GROUPING is not set,,g' \
		$(UCLIBC_DIR)/.config
  else
	$(SED) -i -e 's,^# UCLIBC_HAS_LOCALE is not set,UCLIBC_HAS_LOCALE=y\n# UCLIBC_BUILD_ALL_LOCALE is not set\nUCLIBC_BUILD_MINIMAL_LOCALE=y\n# UCLIBC_PREGENERATED_LOCALE_DATA is not set\nUCLIBC_BUILD_MINIMAL_LOCALES="en_US es_ES de_DE fr_FR"\nUCLIBC_HAS_XLOCALE=y\n# UCLIBC_HAS_GLIBC_DIGIT_GROUPING is not set,g' \
		$(UCLIBC_DIR)/.config
  endif
  # Enable/Disable backtrace support
  ifeq ($(CONFIG_UCLIBC_WITH_BACKTRACE),y)
	$(SED) -i -e 's,^# UCLIBC_HAS_BACKTRACE is not set,UCLIBC_HAS_BACKTRACE=y,g' \
		$(UCLIBC_DIR)/.config
  else
	$(SED) -i -e 's,^UCLIBC_HAS_BACKTRACE=y,# UCLIBC_HAS_BACKTRACE is not set,g' \
		$(UCLIBC_DIR)/.config
  endif
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_configured

#
# install headers for gcc stage 1
#

$(STAGEFILES_DIR)/.uclibc_headers_installed: $(STAGEFILES_DIR)/.uclibc_configured
	PATH='$(PREFIX_BIN):$(PATH)' \
		$(MAKE) -j1 -C $(UCLIBC_DIR) PREFIX=$(TARGET_ROOT) headers install_headers
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_headers_installed

#
# compile uClibc
#

$(STAGEFILES_DIR)/.uclibc_compiled:  $(STAGEFILES_DIR)/.uclibc_configured \
                                     $$(GCC_STAGE1_INSTALLED)
	$(CP) $(UCLIBC_DIR)/.config $(UCLIBC_DIR)/m7x0_config
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(UCLIBC_DIR) distclean
	$(MV) $(UCLIBC_DIR)/m7x0_config $(UCLIBC_DIR)/.config
	PATH='$(GCC_STAGE1_PREFIX_BIN):$(PREFIX_BIN):$(PATH)' \
		$(MAKE) -j1 -C $(UCLIBC_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_compiled

#
# install uClibc
#

$(STAGEFILES_DIR)/.uclibc_installed: $(STAGEFILES_DIR)/.uclibc_compiled
	PATH='$(GCC_STAGE1_PREFIX_BIN):$(PREFIX_BIN):$(PATH)' \
		$(MAKE) -j1 -C $(UCLIBC_DIR) PREFIX=$(TARGET_ROOT) install
	echo "#define O7OTOOLCHAINVERSION \"$(UCLIBC_VERSION)" \
		"`$(CAT) $(UCLIBC_CONFIG) $(wildcard $(UCLIBC_PATCHES_DIR)/*.patch) \
			| $(MD5) | $(CUT) -d " " -f 1`\"" \
		> $(TARGET_ROOT)/usr/include/$(UCLIBC_O7OVERSION_H)
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_installed


$(STAGEFILES_DIR)/.uclibc_utils_installed: $$(GCC_INSTALLED) \
                                          $(STAGEFILES_DIR)/.uclibc_installed
	PATH='$(PREFIX_BIN):$(PATH)' \
		$(MAKE) -j -C $(UCLIBC_DIR) PREFIX=$(TARGET_ROOT) utils install_utils
#
# unsupported os/arch
#
#
#ifeq ($(strip $(HOST_BS)),OpenBSD)
#    ifneq ($(strip $(HOST_ARCH)),i386)
#	$(SECHO)
#	$(SECHO) Sorry, hostutils are not supported for $(HOST_BS)/$(HOST_ARCH)
#	$(SECHO) caused of none COMPAT_LINUX kernel option
#	$(SECHO)
#    endif
#else
#ifeq ($(strip $(HOST_BS)),Darwin)
#	$(SECHO)
#	$(SECHO) Sorry, hostutils are not supported for $(HOST_BS)/$(HOST_ARCH)
#	$(SECHO)
#else
#	PATH='$(PREFIX_BIN):$(PATH)' \
#		$(MAKE) -C $(UCLIBC_DIR)/utils hostutils
#	$(CP) $(UCLIBC_DIR)/utils/ldd.host $(PREFIX_BIN)/$(UCLIBC_TARGET)-ldd
#	$(CP) $(UCLIBC_DIR)/utils/ldconfig.host $(PREFIX_BIN)/$(UCLIBC_TARGET)-ldconfig
#	$(CP) $(UCLIBC_DIR)/utils/readelf.host $(PREFIX_BIN)/$(UCLIBC_TARGET)-readelf.uclibc
#endif
#endif
	($(CD) $(PREFIX_BIN); \
		for tool in mips-linux-uclibc-* ; do \
		   $(LN) -sf $$tool `$(ECHO) $$tool | sed 's/mips-linux-uclibc-/mips-linux-/'`; \
		done)
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_utils_installed

$(FILELIST_DIR)/uclibc.lst: $(STAGEFILES_DIR)/.uclibc_utils_installed
	$(TOUCH) $(FILELIST_DIR)/uclibc.lst

.PHONY: clean-uclibc distclean-uclibc

clean-uclibc:
	-$(RM) -rf $(UCLIBC_DIR)

distclean-uclibc:
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_installed
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_utils_installed
	-$(RM) -f $(STAGEFILES_DIR)/.uclibc_headers_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UCLIBC_DLFILE)
endif
