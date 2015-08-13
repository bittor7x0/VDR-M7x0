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

# Put dependencies here
ISL_HOSTDEPS = $(GMP_HOSTINSTALLED)

ISL_HOSTVERSION := 0.15
ISL_HOSTPATCHES_DIR := $(PATCHES_DIR)/isl/$(ISL_HOSTVERSION)

ISL_HOSTFILE := isl-$(ISL_HOSTVERSION).tar.bz2
ISL_HOSTDLFILE := $(DOWNLOAD_DIR)/$(ISL_HOSTFILE)
ISL_HOSTURL := http://isl.gforge.inria.fr/$(ISL_HOSTFILE)
ISL_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/isl-$(ISL_HOSTVERSION)

ISL_HOSTINSTALLED = $(STAGEFILES_DIR)/.isl_host_installed

PACKS_RULES_$(CONFIG_ISL) += $(ISL_HOSTINSTALLED)

CLEAN_RULES += clean-isl
DISTCLEAN_RULES += distclean-isl

#
# download isl
#

$(ISL_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(ISL_HOSTDLFILE) ] ; then \
	$(WGET) $(ISL_HOSTURL) -O $(ISL_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(ISL_HOSTDLFILE)

#
# unpack isl
#

$(STAGEFILES_DIR)/.isl_host_unpacked: $(ISL_HOSTDLFILE) \
                                           $(wildcard $(ISL_HOSTPATCHES_DIR)/*.patch) \
                                           $$(ISL_HOSTDEPS)
	-$(RM) -rf $(ISL_HOSTDIR)
	$(BZCAT) $(ISL_HOSTDLFILE) | $(TAR) -C $(HOSTUTILS_BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.isl_host_unpacked

#
# patch isl
#

$(STAGEFILES_DIR)/.isl_host_patched: $(STAGEFILES_DIR)/.isl_host_unpacked
	$(call patch_package, $(ISL_HOSTDIR), $(ISL_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.isl_host_patched

#
# configure isl
#

$(STAGEFILES_DIR)/.isl_host_configured: $(STAGEFILES_DIR)/.isl_host_patched
	($(CD) $(ISL_HOSTDIR) ; $(ISL_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-clang=no \
			--with-gmp=system \
			--with-gmp-prefix=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.isl_host_configured

#
# compile isl
#

$(STAGEFILES_DIR)/.isl_host_compiled: $(STAGEFILES_DIR)/.isl_host_configured
	$(MAKE) -C $(ISL_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.isl_host_compiled

#
# install isl
#

$(STAGEFILES_DIR)/.isl_host_installed: $(STAGEFILES_DIR)/.isl_host_compiled
	$(MAKE) -C $(ISL_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.isl_host_installed


.PHONY: clean-isl distclean-isl

clean-isl:
	-$(RM) -rf $(ISL_HOSTDIR)

#
# clean everthing else
#

distclean-isl:
	-$(RM) -f $(STAGEFILES_DIR)/.isl_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.isl_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.isl_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.isl_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.isl_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(ISL_HOSTDLFILE)
endif
