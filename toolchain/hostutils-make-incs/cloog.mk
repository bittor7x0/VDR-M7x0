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

# Put dependencies here
CLOOG_HOSTDEPS = $(ISL_HOSTINSTALLED) $(GMP_HOSTINSTALLED)

CLOOG_HOSTVERSION := 0.20.0
CLOOG_HOSTPATCHES_DIR := $(PATCHES_DIR)/cloog/$(CLOOG_HOSTVERSION)

CLOOG_HOSTFILE := cloog-$(CLOOG_HOSTVERSION).tar.gz
CLOOG_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CLOOG_HOSTFILE)
# Old version (0.18.4) from official website
#CLOOG_HOSTURL := http://www.bastoul.net/cloog/pages/download/$(CLOOG_HOSTFILE)
# New version (0.20.0) from github with isl 0.20 support
CLOOG_HOSTURL := https://github.com/periscop/cloog/releases/download/cloog-$(CLOOG_HOSTVERSION)/$(CLOOG_HOSTFILE)
CLOOG_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/cloog-$(CLOOG_HOSTVERSION)

CLOOG_HOSTINSTALLED = $(STAGEFILES_DIR)/.cloog_host_installed

PACKS_RULES_$(CONFIG_CLOOG) += $(CLOOG_HOSTINSTALLED)

CLEAN_RULES += clean-cloog
DISTCLEAN_RULES += distclean-cloog

#
# download cloog
#

$(CLOOG_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CLOOG_HOSTDLFILE) ] ; then \
	$(WGET) $(CLOOG_HOSTURL) -O $(CLOOG_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CLOOG_HOSTDLFILE)

#
# unpack cloog
#

$(STAGEFILES_DIR)/.cloog_host_unpacked: $(CLOOG_HOSTDLFILE) \
                                           $(wildcard $(CLOOG_HOSTPATCHES_DIR)/*.patch) \
                                           $$(CLOOG_HOSTDEPS)
	-$(RM) -rf $(CLOOG_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(CLOOG_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_unpacked

#
# patch cloog
#

$(STAGEFILES_DIR)/.cloog_host_patched: $(STAGEFILES_DIR)/.cloog_host_unpacked
	$(call patch_package, $(CLOOG_HOSTDIR), $(CLOOG_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_patched

#
# configure cloog
#

$(STAGEFILES_DIR)/.cloog_host_configured: $(STAGEFILES_DIR)/.cloog_host_patched
	($(CD) $(CLOOG_HOSTDIR) ; $(CLOOG_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-host-libstdcxx='-lstdc++' \
			--with-osl=no \
			--with-isl=system \
			--with-isl-prefix=$(PREFIX) \
			--with-bits=gmp \
			--with-gmp=system \
			--with-gmp-prefix=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_configured

#
# compile cloog
#

$(STAGEFILES_DIR)/.cloog_host_compiled: $(STAGEFILES_DIR)/.cloog_host_configured
	$(MAKE) -C $(CLOOG_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_compiled

#
# install cloog
#

$(STAGEFILES_DIR)/.cloog_host_installed: $(STAGEFILES_DIR)/.cloog_host_compiled
	$(MAKE) -C $(CLOOG_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_installed


.PHONY: clean-cloog distclean-cloog

clean-cloog:
	-$(RM) -rf $(CLOOG_HOSTDIR)

#
# clean everthing else
#

distclean-cloog:
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CLOOG_HOSTDLFILE)
endif
