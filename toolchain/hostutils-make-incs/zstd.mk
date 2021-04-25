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

ZSTD_HOSTVERSION := 1.4.9
ZSTD_HOSTPATCHES_DIR := $(PATCHES_DIR)/zstd/$(ZSTD_HOSTVERSION)

ZSTD_HOSTFILE := zstd-$(ZSTD_HOSTVERSION).tar.gz
ZSTD_HOSTDLFILE := $(DOWNLOAD_DIR)/$(ZSTD_HOSTFILE)
ZSTD_HOSTURL := https://github.com/facebook/zstd/releases/download/v$(ZSTD_HOSTVERSION)/$(ZSTD_HOSTFILE)
ZSTD_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/zstd-$(ZSTD_HOSTVERSION)

ZSTD_HOSTINSTALLED = $(STAGEFILES_DIR)/.zstd_host_installed

PACKS_RULES_$(CONFIG_ZSTD) += $(ZSTD_HOSTINSTALLED)

CLEAN_RULES += clean-zstd
DISTCLEAN_RULES += distclean-zstd

#
# download zstd
#

$(ZSTD_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(ZSTD_HOSTDLFILE) ] ; then \
	$(WGET) $(ZSTD_HOSTURL) -O $(ZSTD_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(ZSTD_HOSTDLFILE)

#
# unpack zstd
#

$(STAGEFILES_DIR)/.zstd_host_unpacked: $(ZSTD_HOSTDLFILE) \
                                           $(wildcard $(ZSTD_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(ZSTD_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(ZSTD_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.zstd_host_unpacked

#
# patch zstd
#

$(STAGEFILES_DIR)/.zstd_host_patched: $(STAGEFILES_DIR)/.zstd_host_unpacked
	$(call patch_package, $(ZSTD_HOSTDIR), $(ZSTD_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.zstd_host_patched

#
# compile zstd
#

$(STAGEFILES_DIR)/.zstd_host_compiled: $(STAGEFILES_DIR)/.zstd_host_patched
	$(MAKE) -C $(ZSTD_HOSTDIR) PREFIX=$(HOSTUTILS_PREFIX) HAVE_ZLIB=0 HAVE_LZMA=0 HAVE_LZ4=0
	$(TOUCH) $(STAGEFILES_DIR)/.zstd_host_compiled

#
# install zstd
#

$(STAGEFILES_DIR)/.zstd_host_installed: $(STAGEFILES_DIR)/.zstd_host_compiled
	$(MAKE) -C $(ZSTD_HOSTDIR) PREFIX=$(HOSTUTILS_PREFIX) install
	$(TOUCH) $(STAGEFILES_DIR)/.zstd_host_installed


.PHONY: clean-zstd distclean-zstd

clean-zstd:
	-$(RM) -rf $(ZSTD_HOSTDIR)

#
# clean everthing else
#

distclean-zstd:
	-$(RM) -f $(STAGEFILES_DIR)/.zstd_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.zstd_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.zstd_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.zstd_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.zstd_host_installed
	-$(RM) -rf $(HOSTUTILS_PREFIX_BIN)/{unzstd,zstd,zstdcat,zstdgrep,zstdless,zstdmt}
	-$(RM) -f $(HOSTUTILS_PREFIX)/lib/libzstd.a
	-$(RM) -f $(HOSTUTILS_PREFIX)/lib/libzstd.so*
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(ZSTD_HOSTDLFILE)
endif
