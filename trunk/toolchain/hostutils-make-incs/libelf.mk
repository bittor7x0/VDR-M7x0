# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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

LIBELF_HOSTVERSION := 0.8.13
LIBELF_HOSTPATCHES_DIR := $(PATCHES_DIR)/libelf/$(LIBELF_HOSTVERSION)

LIBELF_HOSTFILE := libelf-$(LIBELF_HOSTVERSION).tar.gz
LIBELF_HOSTDLFILE := $(DOWNLOAD_DIR)/$(LIBELF_HOSTFILE)
LIBELF_HOSTURL := http://www.mr511.de/software/$(LIBELF_HOSTFILE)
LIBELF_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/libelf-$(LIBELF_HOSTVERSION)

LIBELF_HOSTINSTALLED = $(STAGEFILES_DIR)/.libelf_host_installed

PACKS_RULES_$(CONFIG_LIBELF) += $(LIBELF_HOSTINSTALLED)

CLEAN_RULES += clean-libelf
DISTCLEAN_RULES += distclean-libelf

#
# download libelf
#

$(LIBELF_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBELF_HOSTDLFILE) ] ; then \
	$(WGET) $(LIBELF_HOSTURL) -O $(LIBELF_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(LIBELF_HOSTDLFILE)

#
# unpack libelf
#

$(STAGEFILES_DIR)/.libelf_host_unpacked: $(LIBELF_HOSTDLFILE) \
                                           $(wildcard $(LIBELF_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(LIBELF_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(LIBELF_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.libelf_host_unpacked

#
# patch libelf
#

$(STAGEFILES_DIR)/.libelf_host_patched: $(STAGEFILES_DIR)/.libelf_host_unpacked
	$(call patch_package, $(LIBELF_HOSTDIR), $(LIBELF_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libelf_host_patched

#
# configure libelf
#

$(STAGEFILES_DIR)/.libelf_host_configured: $(STAGEFILES_DIR)/.libelf_host_patched
	($(CD) $(LIBELF_HOSTDIR) ; $(LIBELF_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--disable-elf64 \
			--disable-nls)
	$(TOUCH) $(STAGEFILES_DIR)/.libelf_host_configured

#
# compile libelf
#

$(STAGEFILES_DIR)/.libelf_host_compiled: $(STAGEFILES_DIR)/.libelf_host_configured
	$(MAKE) -C $(LIBELF_HOSTDIR)/lib/ libelf.a
	$(TOUCH) $(STAGEFILES_DIR)/.libelf_host_compiled

#
# install libelf
#

$(STAGEFILES_DIR)/.libelf_host_installed: $(STAGEFILES_DIR)/.libelf_host_compiled
	$(MAKE) -C $(LIBELF_HOSTDIR)/lib/ install
	$(TOUCH) $(STAGEFILES_DIR)/.libelf_host_installed


.PHONY: clean-libelf distclean-libelf

clean-libelf:
	-$(RM) -rf $(LIBELF_HOSTDIR)

#
# clean everthing else
#

distclean-libelf:
	-$(RM) -f $(STAGEFILES_DIR)/.libelf_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libelf_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libelf_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libelf_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libelf_host_installed
	-$(RM) -rf $(HOSTUTILS_PREFIX_BIN)/include/libelf
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/lib/libelf.a
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBELF_HOSTDLFILE)
endif
