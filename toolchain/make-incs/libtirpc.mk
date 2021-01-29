# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2021 VDR-NG-EM Project
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
LIBTIRPC_DEPS = $(BASE_BUILD_STAGEFILE)

LIBTIRPC_VERSION := 1.2.6
LIBTIRPC_PATCHES_DIR := $(PATCHES_DIR)/libtirpc/$(LIBTIRPC_VERSION)

LIBTIRPC_FILE := libtirpc-$(LIBTIRPC_VERSION).tar.bz2
LIBTIRPC_DLFILE := $(DOWNLOAD_DIR)/$(LIBTIRPC_FILE)
LIBTIRPC_URL := http://downloads.sourceforge.net/libtirpc/$(LIBTIRPC_FILE)
LIBTIRPC_DIR := $(BUILD_DIR)/libtirpc-$(LIBTIRPC_VERSION)

LIBTIRPC_INSTALLED = $(STAGEFILES_DIR)/.libtirpc_installed

PACKS_RULES_$(CONFIG_LIBTIRPC) += $(LIBTIRPC_INSTALLED)

FILE_LISTS_$(CONFIG_LIBTIRPC) += libtirpc.lst

CLEAN_RULES += clean-libtirpc
DISTCLEAN_RULES += distclean-libtirpc

#
# download libtirpc
#

$(LIBTIRPC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBTIRPC_DLFILE) ] ; then \
	$(WGET) $(LIBTIRPC_URL) -O $(LIBTIRPC_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBTIRPC_DLFILE)

#
# unpack libtirpc
#

$(STAGEFILES_DIR)/.libtirpc_unpacked: $(LIBTIRPC_DLFILE) \
                                           $(wildcard $(LIBTIRPC_PATCHES_DIR)/*.patch) \
                                           $$(LIBTIRPC_DEPS)
	-$(RM) -rf $(LIBTIRPC_DIR)
	$(BZCAT) $(LIBTIRPC_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.libtirpc_unpacked

#
# patch libtirpc
#

$(STAGEFILES_DIR)/.libtirpc_patched: $(STAGEFILES_DIR)/.libtirpc_unpacked
	$(call patch_package, $(LIBTIRPC_DIR), $(LIBTIRPC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libtirpc_patched

#
# configure libtirpc
#

$(STAGEFILES_DIR)/.libtirpc_configured: $(STAGEFILES_DIR)/.libtirpc_patched
	($(CD) $(LIBTIRPC_DIR) ; $(UCLIBC_ENV_LTO_GC_LOOPS) \
		$(LIBTIRPC_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-ipv6 \
			--disable-gssapi \
			--with-gnu-ld)
	$(TOUCH) $(STAGEFILES_DIR)/.libtirpc_configured

#
# compile libtirpc
#

$(STAGEFILES_DIR)/.libtirpc_compiled: $(STAGEFILES_DIR)/.libtirpc_configured
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(LIBTIRPC_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libtirpc_compiled

#
# install libtirpc
#

$(STAGEFILES_DIR)/.libtirpc_installed: $(STAGEFILES_DIR)/.libtirpc_compiled
	$(UCLIBC_ENV_LTO_GC_LOOPS) $(MAKE) -C $(LIBTIRPC_DIR) install
	$(SED) -i -e '/^udp6/d' $(TARGET_ROOT)/usr/etc/netconfig
	$(SED) -i -e '/^tcp6/d' $(TARGET_ROOT)/usr/etc/netconfig
	$(TOUCH) $(STAGEFILES_DIR)/.libtirpc_installed


$(FILELIST_DIR)/libtirpc.lst: $(STAGEFILES_DIR)/.libtirpc_installed
	$(TOUCH) $(FILELIST_DIR)/libtirpc.lst


.PHONY: clean-libtirpc distclean-libtirpc

clean-libtirpc:
	-$(RM) -rf $(LIBTIRPC_DIR)

#
# clean everthing else
#

distclean-libtirpc:
	-$(RM) -f $(STAGEFILES_DIR)/.libtirpc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libtirpc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libtirpc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libtirpc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libtirpc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBTIRPC_DLFILE)
endif
