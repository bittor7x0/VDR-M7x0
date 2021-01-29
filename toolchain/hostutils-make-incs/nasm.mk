# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2019-2021 VDR-NG-EM Project
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

# Put dependencies here
NASM_HOSTDEPS = $(GMP_HOSTINSTALLED)

NASM_HOSTVERSION := 2.15.05
NASM_HOSTPATCHES_DIR := $(PATCHES_DIR)/nasm/$(NASM_HOSTVERSION)

NASM_HOSTFILE := nasm-$(NASM_HOSTVERSION).tar.xz
NASM_HOSTDLFILE := $(DOWNLOAD_DIR)/$(NASM_HOSTFILE)
NASM_HOSTURL := https://www.nasm.us/pub/nasm/releasebuilds/$(NASM_HOSTVERSION)/$(NASM_HOSTFILE)
NASM_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/nasm-$(NASM_HOSTVERSION)

NASM_HOSTINSTALLED = $(STAGEFILES_DIR)/.nasm_host_installed

PACKS_RULES_$(CONFIG_NASM) += $(NASM_HOSTINSTALLED)

CLEAN_RULES += clean-nasm
DISTCLEAN_RULES += distclean-nasm

#
# download nasm
#

$(NASM_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NASM_HOSTDLFILE) ] ; then \
	$(WGET) $(NASM_HOSTURL) -O $(NASM_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(NASM_HOSTDLFILE)

#
# unpack nasm
#

$(STAGEFILES_DIR)/.nasm_host_unpacked: $(NASM_HOSTDLFILE) \
                                           $(wildcard $(NASM_HOSTPATCHES_DIR)/*.patch) \
                                           $$(NASM_HOSTDEPS)
	-$(RM) -rf $(NASM_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -xJf $(NASM_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.nasm_host_unpacked

#
# patch nasm
#

$(STAGEFILES_DIR)/.nasm_host_patched: $(STAGEFILES_DIR)/.nasm_host_unpacked
	$(call patch_package, $(NASM_HOSTDIR), $(NASM_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.nasm_host_patched

#
# configure nasm
#

$(STAGEFILES_DIR)/.nasm_host_configured: $(STAGEFILES_DIR)/.nasm_host_patched
	($(CD) $(NASM_HOSTDIR) ; $(NASM_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--target=$(TARGET) \
			--disable-lto \
			--disable-werror \
			--disable-gdb)
	$(TOUCH) $(STAGEFILES_DIR)/.nasm_host_configured

#
# compile nasm
#

$(STAGEFILES_DIR)/.nasm_host_compiled: $(STAGEFILES_DIR)/.nasm_host_configured
	$(MAKE) -C $(NASM_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.nasm_host_compiled

#
# install nasm
#

$(STAGEFILES_DIR)/.nasm_host_installed: $(STAGEFILES_DIR)/.nasm_host_compiled
	$(MAKE) -C $(NASM_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.nasm_host_installed


.PHONY: clean-nasm distclean-nasm

clean-nasm:
	-$(RM) -rf $(NASM_HOSTDIR)

#
# clean everthing else
#

distclean-nasm:
	-$(RM) -f $(STAGEFILES_DIR)/.nasm_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.nasm_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.nasm_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.nasm_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.nasm_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NASM_HOSTDLFILE)
endif
