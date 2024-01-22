# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2024 VDR-NG-EM Project
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

UCLIBC_IS_NG := y
UCLIBC_IS_SNAPSHOT := n

UCLIBC_O7OVERSION_H := o7o-toolchain-version.h

ifeq ($(UCLIBC_IS_NG),y)
  ifeq ($(UCLIBC_IS_SNAPSHOT),y)
	# http://repo.or.cz/uclibc-ng.git/commit/de6be7bc60f190a0d746945a3a5a143bc93a1a65
	UCLIBC_VERSION := de6be7b
	UCLIBC_VERSION_FULL := de6be7bc60f190a0d746945a3a5a143bc93a1a65
	UCLIBC_FILE := uClibc-ng-$(UCLIBC_VERSION).tar.gz
	UCLIBC_DIR := $(BUILD_DIR)/uclibc-ng-$(UCLIBC_VERSION)
	UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc-ng/snapshot/uclibc-ng.config
	UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc-ng/snapshot
	UCLIBC_URL := http://repo.or.cz/uclibc-ng.git/snapshot/$(UCLIBC_VERSION_FULL).tar.gz
	UCLIBC_FILE_LIST += uclibc-ng-snapshot.lst
  else
	UCLIBC_VERSION := 1.0.45
	UCLIBC_FILE := uClibc-ng-$(UCLIBC_VERSION).tar.bz2
	UCLIBC_DIR := $(BUILD_DIR)/uClibc-ng-$(UCLIBC_VERSION)
	UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc-ng/$(UCLIBC_VERSION)/uclibc-ng.config
	UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc-ng/$(UCLIBC_VERSION)
	UCLIBC_URL := https://downloads.uclibc-ng.org/releases/$(UCLIBC_VERSION)/$(UCLIBC_FILE)
	UCLIBC_FILE_LIST += uclibc-ng.lst
  endif
else
  ifeq ($(UCLIBC_IS_SNAPSHOT),y)
	# http://git.uclibc.org/uClibc/commit/?id=ca1c74d67dd115d059a875150e10b8560a9c35a8
	UCLIBC_VERSION := ca1c74d67dd115d059a875150e10b8560a9c35a8
	UCLIBC_FILE := uClibc-$(UCLIBC_VERSION).tar.gz
	UCLIBC_DIR := $(BUILD_DIR)/uClibc-$(UCLIBC_VERSION)
	UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc/snapshot/uclibc.config
	UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc/snapshot
	UCLIBC_URL := http://git.uclibc.org/uClibc/snapshot/$(UCLIBC_FILE)
	UCLIBC_FILE_LIST += uclibc-snapshot.lst
  else
	UCLIBC_VERSION := 0.9.30.3
	UCLIBC_FILE := uClibc-$(UCLIBC_VERSION).tar.bz2
	UCLIBC_DIR := $(BUILD_DIR)/uClibc-$(UCLIBC_VERSION)
	UCLIBC_CONFIG := $(CONFIGS_DIR)/uClibc/$(UCLIBC_VERSION)/uclibc.config
	UCLIBC_PATCHES_DIR := $(PATCHES_DIR)/uClibc/$(UCLIBC_VERSION)
	UCLIBC_URL := http://www.uclibc.org/downloads/$(UCLIBC_FILE)
	UCLIBC_FILE_LIST += uclibc.lst
  endif
endif

UCLIBC_DLFILE := $(DOWNLOAD_DIR)/$(UCLIBC_FILE)

UCLIBC_PRE_ALL_GCC = $(UCLIBC_DLFILE) $(wildcard $(UCLIBC_PATCHES_DIR)/*.patch) $(UCLIBC_CONFIG) \
   $(STAGEFILES_DIR)/.uclibc_headers_installed
UCLIBC_INSTALLED = $(STAGEFILES_DIR)/.uclibc_installed

BASE_RULES_y += $(STAGEFILES_DIR)/.uclibc_utils_installed
CLEAN_RULES += clean-uclibc
DISTCLEAN_RULES += distclean-uclibc
FILE_LISTS_y += $(UCLIBC_FILE_LIST)

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
ifeq ($(UCLIBC_IS_SNAPSHOT),y)
	$(TAR) -C $(BUILD_DIR) -zf $(UCLIBC_DLFILE)
else
	$(TAR) -C $(BUILD_DIR) -jf $(UCLIBC_DLFILE)
endif
ifeq ($(UCLIBC_IS_NG),y)
	-$(RM) -rf $(UCLIBC_DIR)/libpthread/linuxthreads $(UCLIBC_DIR)/libpthread/linuxthreads_db
	$(TAR) -C $(UCLIBC_DIR) -xJf $(BUILDIN_DIR)/uClibc-ng/linuxthreads.new.tar.xz
# Only NFS support requires an RPC implementation (BusyBox mount, nfs-utils and portmap/rpcbind packages)
# If libtirpc isn't enabled, we'll use the RPC implementation removed in this commit:
# http://cgit.uclibc-ng.org/cgi/cgit/uclibc-ng.git/commit/?id=a1a8064169aeda79e3266a2db9cce25e361a86dc
ifeq ($(CONFIG_FW_VERSION),pro)
ifneq ($(CONFIG_LIBTIRPC),y)
	$(TAR) -C $(UCLIBC_DIR) -xJf $(BUILDIN_DIR)/uClibc-ng/rpc.tar.xz
endif
endif
endif
	$(TOUCH) $(STAGEFILES_DIR)/.uclibc_unpacked

#
# patch uClibc
#

$(STAGEFILES_DIR)/.uclibc_patched: $(STAGEFILES_DIR)/.uclibc_unpacked
ifeq ($(UCLIBC_IS_NG),y)
	$(call patch_package, $(UCLIBC_DIR), $(UCLIBC_PATCHES_DIR)/common)
	$(call patch_package, $(UCLIBC_DIR), $(UCLIBC_PATCHES_DIR)/linuxthreads)
ifeq ($(CONFIG_FW_VERSION),pro)
ifneq ($(CONFIG_LIBTIRPC),y)
	$(call patch_package, $(UCLIBC_DIR), $(UCLIBC_PATCHES_DIR)/rpc)
endif
endif
else
	$(call patch_package, $(UCLIBC_DIR), $(UCLIBC_PATCHES_DIR))
endif
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
  # Enable/Disable RPC implementation
  ifeq ($(or $(filter lite,$(CONFIG_FW_VERSION)),$(filter y,$(CONFIG_LIBTIRPC))),y)
	$(SED) -i -e 's,^UCLIBC_HAS_RPC=y,# UCLIBC_HAS_RPC is not set,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^UCLIBC_HAS_FULL_RPC=y,# UCLIBC_HAS_FULL_RPC is not set,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^UCLIBC_HAS_REENTRANT_RPC=y,# UCLIBC_HAS_REENTRANT_RPC is not set,g' \
		$(UCLIBC_DIR)/.config
  else
	$(SED) -i -e 's,^# UCLIBC_HAS_RPC is not set,UCLIBC_HAS_RPC=y,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^# UCLIBC_HAS_FULL_RPC is not set,UCLIBC_HAS_FULL_RPC=y,g' \
		$(UCLIBC_DIR)/.config
	$(SED) -i -e 's,^# UCLIBC_HAS_REENTRANT_RPC is not set,UCLIBC_HAS_REENTRANT_RPC=y,g' \
		$(UCLIBC_DIR)/.config
  endif
  # libtirpc needs AI_ADDRCONFIG support
  ifeq ($(CONFIG_LIBTIRPC),y)
	$(SED) -i -e 's,^# UCLIBC_SUPPORT_AI_ADDRCONFIG is not set,UCLIBC_SUPPORT_AI_ADDRCONFIG=y,g' \
		$(UCLIBC_DIR)/.config
  else
	$(SED) -i -e 's,^UCLIBC_SUPPORT_AI_ADDRCONFIG=y,# UCLIBC_SUPPORT_AI_ADDRCONFIG is not set,g' \
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
ifeq ($(UCLIBC_IS_NG),y)
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libcrypt.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libcrypt.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libdl.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libdl.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libm.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libm.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libnsl.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libnsl.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libpthread.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libpthread.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libresolv.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libresolv.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/librt.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/librt.so.0
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libutil.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libutil.so.0
  ifeq ($(CONFIG_UCLIBC_WITH_BACKTRACE),y)
	$(LN) -sf libc.so.1 $(TARGET_ROOT)/lib/libubacktrace.so.1
	$(LN) -sf libc.so.0 $(TARGET_ROOT)/lib/libubacktrace.so.0
  endif
endif
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

$(FILELIST_DIR)/$(UCLIBC_FILE_LIST): $(STAGEFILES_DIR)/.uclibc_utils_installed
	$(TOUCH) $(FILELIST_DIR)/$(UCLIBC_FILE_LIST)

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
