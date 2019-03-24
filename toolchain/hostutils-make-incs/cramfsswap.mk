# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
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

CRAMFSSWAP_BIN := $(firstword $(wildcard $(addsuffix /cramfsswap, $(subst :, ,$(PATH)))))

ifeq ($(CRAMFSSWAP_BIN),)
CRAMFSSWAP_BIN := $(HOSTUTILS_PREFIX_BIN)/cramfsswap
endif

CRAMFSSWAP_HOSTVERSION := 1.3.1
CRAMFSSWAP_HOSTFILE := cramfsswap_$(CRAMFSSWAP_HOSTVERSION).tar.gz
CRAMFSSWAP_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CRAMFSSWAP_HOSTFILE)

CRAMFSSWAP_HOSTPATCHES_DIR := $(PATCHES_DIR)/cramfsswap/$(CRAMFSSWAP_HOSTVERSION)
CRAMFSSWAP_HOSTURL := http://old-releases.ubuntu.com/ubuntu/pool/universe/c/cramfsswap/$(CRAMFSSWAP_HOSTFILE)
CRAMFSSWAP_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/cramfsswap-$(CRAMFSSWAP_HOSTVERSION)

CLEAN_RULES += clean-cramfsswap-host
DISTCLEAN_RULES += distclean-cramfsswap-host

#
# download cramfsswap
#

$(CRAMFSSWAP_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CRAMFSSWAP_HOSTDLFILE) ] ; then \
	$(WGET) $(CRAMFSSWAP_HOSTURL) -O $(CRAMFSSWAP_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CRAMFSSWAP_HOSTDLFILE)

#
# unpack cramfsswap
#

$(STAGEFILES_DIR)/.cramfsswap_host_unpacked: $(CRAMFSSWAP_HOSTDLFILE) \
                                           $(wildcard $(CRAMFSSWAP_HOSTPATCHES_DIR)/common/*.patch) \
                                           $(wildcard $(CRAMFSSWAP_HOSTPATCHES_DIR)/host/$(HOST_BS)/*.patch)
	-$(RM) -rf $(CRAMFSSWAP_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(CRAMFSSWAP_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.cramfsswap_host_unpacked

#
# patch cramfsswap
#

$(STAGEFILES_DIR)/.cramfsswap_host_patched: $(STAGEFILES_DIR)/.cramfsswap_host_unpacked
	$(call patch_package, $(CRAMFSSWAP_HOSTDIR), $(CRAMFSSWAP_HOSTPATCHES_DIR)/common)
	$(call patch_package, $(CRAMFSSWAP_HOSTDIR), $(CRAMFSSWAP_HOSTPATCHES_DIR)/host/$(HOST_BS))
ifeq ($(HOST_CYGWIN),y)
	$(call patch_package, $(CRAMFSSWAP_HOSTDIR), $(CRAMFSSWAP_HOSTPATCHES_DIR)/host/OpenBSD)
endif
	$(TOUCH) $(STAGEFILES_DIR)/.cramfsswap_host_patched
#
# compile cramfsswap
#

$(STAGEFILES_DIR)/.cramfsswap_host_compiled: $(STAGEFILES_DIR)/.cramfsswap_host_patched
	$(MAKE) -C $(CRAMFSSWAP_HOSTDIR) clean
	$(MAKE) -C $(CRAMFSSWAP_HOSTDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.cramfsswap_host_compiled

#
# install cramfsswap
#

$(HOSTUTILS_PREFIX_BIN)/cramfsswap: $(STAGEFILES_DIR)/.cramfsswap_host_compiled
	$(CP) $(CRAMFSSWAP_HOSTDIR)/cramfsswap $(HOSTUTILS_PREFIX_BIN)/cramfsswap

.PHONY: clean-cramfsswap-host distclean-cramfsswap-host

clean-cramfsswap-host:
	-$(RM) -rf $(CRAMFSSWAP_HOSTDIR)

distclean-cramfsswap-host:
	-$(RM) -f $(STAGEFILES_DIR)/.cramfsswap_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.cramfsswap_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.cramfsswap_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/cramfsswap
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CRAMFSSWAP_HOSTDLFILE)
endif
