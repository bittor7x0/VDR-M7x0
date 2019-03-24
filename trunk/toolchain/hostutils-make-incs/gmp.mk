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

GMP_HOSTVERSION := 6.1.2
GMP_HOSTPATCHES_DIR := $(PATCHES_DIR)/gmp/$(GMP_HOSTVERSION)

GMP_HOSTFILE := gmp-$(GMP_HOSTVERSION).tar.bz2
GMP_HOSTDLFILE := $(DOWNLOAD_DIR)/$(GMP_HOSTFILE)
GMP_HOSTURL := https://gmplib.org/download/gmp/$(GMP_HOSTFILE)
GMP_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/gmp-$(GMP_HOSTVERSION)

GMP_HOSTINSTALLED = $(STAGEFILES_DIR)/.gmp_host_installed

PACKS_RULES_$(CONFIG_GMP) += $(GMP_HOSTINSTALLED)

CLEAN_RULES += clean-gmp
DISTCLEAN_RULES += distclean-gmp

#
# download gmp
#

$(GMP_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GMP_HOSTDLFILE) ] ; then \
	$(WGET) $(GMP_HOSTURL) -O $(GMP_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(GMP_HOSTDLFILE)

#
# unpack gmp
#

$(STAGEFILES_DIR)/.gmp_host_unpacked: $(GMP_HOSTDLFILE) \
                                           $(wildcard $(GMP_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(GMP_HOSTDIR)
	$(BZCAT) $(GMP_HOSTDLFILE) | $(TAR) -C $(HOSTUTILS_BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_host_unpacked

#
# patch gmp
#

$(STAGEFILES_DIR)/.gmp_host_patched: $(STAGEFILES_DIR)/.gmp_host_unpacked
	$(call patch_package, $(GMP_HOSTDIR), $(GMP_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_host_patched

#
# configure gmp
#

$(STAGEFILES_DIR)/.gmp_host_configured: $(STAGEFILES_DIR)/.gmp_host_patched
	($(CD) $(GMP_HOSTDIR) ; CPPFLAGS=-fexceptions \
		$(GMP_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--enable-cxx)
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_host_configured

#
# compile gmp
#

$(STAGEFILES_DIR)/.gmp_host_compiled: $(STAGEFILES_DIR)/.gmp_host_configured
	$(MAKE) -C $(GMP_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_host_compiled

#
# install gmp
#

$(STAGEFILES_DIR)/.gmp_host_installed: $(STAGEFILES_DIR)/.gmp_host_compiled
	$(MAKE) -C $(GMP_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_host_installed


.PHONY: clean-gmp distclean-gmp

clean-gmp:
	-$(RM) -rf $(GMP_HOSTDIR)

#
# clean everthing else
#

distclean-gmp:
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GMP_HOSTDLFILE)
endif
