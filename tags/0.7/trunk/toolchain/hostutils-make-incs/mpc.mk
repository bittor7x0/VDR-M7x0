# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012 VDR-NG-EM Project
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
MPC_HOSTDEPS = $(GMP_HOSTINSTALLED) $(MPFR_HOSTINSTALLED)

MPC_HOSTVERSION := 0.9
MPC_HOSTPATCHES_DIR := $(PATCHES_DIR)/mpc/$(MPC_HOSTVERSION)

MPC_HOSTFILE := mpc-$(MPC_HOSTVERSION).tar.gz
MPC_HOSTDLFILE := $(DOWNLOAD_DIR)/$(MPC_HOSTFILE)
MPC_HOSTURL := http://www.multiprecision.org/mpc/download/$(MPC_HOSTFILE)
MPC_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/mpc-$(MPC_HOSTVERSION)

MPC_HOSTINSTALLED = $(STAGEFILES_DIR)/.mpc_host_installed

PACKS_RULES_$(CONFIG_MPC) += $(MPC_HOSTINSTALLED)

CLEAN_RULES += clean-mpc
DISTCLEAN_RULES += distclean-mpc

#
# download mpc
#

$(MPC_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MPC_HOSTDLFILE) ] ; then \
	$(WGET) $(MPC_HOSTURL) -O $(MPC_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(MPC_HOSTDLFILE)

#
# unpack mpc
#

$(STAGEFILES_DIR)/.mpc_host_unpacked: $(MPC_HOSTDLFILE) \
                                           $(wildcard $(MPC_HOSTPATCHES_DIR)/*.patch) \
                                           $$(MPC_HOSTDEPS)
	-$(RM) -rf $(MPC_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(MPC_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_host_unpacked

#
# patch mpc
#

$(STAGEFILES_DIR)/.mpc_host_patched: $(STAGEFILES_DIR)/.mpc_host_unpacked
	$(call patch_package, $(MPC_HOSTDIR), $(MPC_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_host_patched

#
# configure mpc
#

$(STAGEFILES_DIR)/.mpc_host_configured: $(STAGEFILES_DIR)/.mpc_host_patched
	($(CD) $(MPC_HOSTDIR) ; $(MPC_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-mpfr=$(PREFIX) \
			--with-gmp=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_host_configured

#
# compile mpc
#

$(STAGEFILES_DIR)/.mpc_host_compiled: $(STAGEFILES_DIR)/.mpc_host_configured
	$(MAKE) -C $(MPC_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_host_compiled

#
# install mpc
#

$(STAGEFILES_DIR)/.mpc_host_installed: $(STAGEFILES_DIR)/.mpc_host_compiled
	$(MAKE) -C $(MPC_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_host_installed


.PHONY: clean-mpc distclean-mpc

clean-mpc:
	-$(RM) -rf $(MPC_HOSTDIR)

#
# clean everthing else
#

distclean-mpc:
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MPC_HOSTDLFILE)
endif
