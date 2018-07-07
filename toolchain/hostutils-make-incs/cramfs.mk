# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2018 VDR-NG-EM Project
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

MKCRAMFS_BIN := $(HOSTUTILS_PREFIX_BIN)/mkcramfs
CRAMFSCK_BIN := $(HOSTUTILS_PREFIX_BIN)/cramfsck


CRAMFS_HOSTVERSION := 1.1
CRAMFS_HOSTFILE := cramfs-$(CRAMFS_HOSTVERSION).tar.gz
CRAMFS_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CRAMFS_HOSTFILE)

CRAMFS_HOSTPATCHES_DIR := $(PATCHES_DIR)/cramfs/$(CRAMFS_HOSTVERSION)
CRAMFS_HOSTURL := http://sourceforge.net/projects/cramfs/files/cramfs/$(CRAMFS_HOSTVERSION)/$(CRAMFS_HOSTFILE)/download
CRAMFS_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/cramfs-$(CRAMFS_HOSTVERSION)

CLEAN_RULES += clean-cramfs-host
DISTCLEAN_RULES += distclean-cramfs-host

#
# download cramfs
#

$(CRAMFS_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CRAMFS_HOSTDLFILE) ] ; then \
	$(WGET) $(CRAMFS_HOSTURL) -O $(CRAMFS_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CRAMFS_HOSTDLFILE)

#
# unpack cramfs
#

$(STAGEFILES_DIR)/.cramfs_host_unpacked: $(CRAMFS_HOSTDLFILE) \
                                         $(wildcard $(CRAMFS_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(CRAMFS_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(CRAMFS_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.cramfs_host_unpacked

#
# patch cramfs
#

$(STAGEFILES_DIR)/.cramfs_host_patched: $(STAGEFILES_DIR)/.cramfs_host_unpacked
	$(call patch_package, $(CRAMFS_HOSTDIR), $(CRAMFS_HOSTPATCHES_DIR)/common)
	$(call patch_package, $(CRAMFS_HOSTDIR), $(CRAMFS_HOSTPATCHES_DIR)/host/$(HOST_BS))
	$(TOUCH) $(STAGEFILES_DIR)/.cramfs_host_patched

#
# compile cramfs
#

$(STAGEFILES_DIR)/.cramfs_host_compiled: $(STAGEFILES_DIR)/.cramfs_host_patched
	$(MAKE) -C $(CRAMFS_HOSTDIR) -f GNUmakefile distclean
	$(MAKE) -C $(CRAMFS_HOSTDIR) -f GNUmakefile all
	$(TOUCH) $(STAGEFILES_DIR)/.cramfs_host_compiled

#
# install cramfs
#

$(HOSTUTILS_PREFIX_BIN)/mkcramfs: $(STAGEFILES_DIR)/.cramfs_host_compiled
	$(CP) $(CRAMFS_HOSTDIR)/mkcramfs $(HOSTUTILS_PREFIX_BIN)/mkcramfs

$(HOSTUTILS_PREFIX_BIN)/cramfsck: $(STAGEFILES_DIR)/.cramfs_host_compiled
	$(CP) $(CRAMFS_HOSTDIR)/cramfsck $(HOSTUTILS_PREFIX_BIN)/cramfsck

.PHONY: clean-cramfs-host distclean-cramfs-host

clean-cramfs-host:
	-$(RM) -rf $(CRAMFS_HOSTDIR)

distclean-cramfs-host:
	-$(RM) -f $(STAGEFILES_DIR)/.cramfs_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.cramfs_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.cramfs_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/mkcramfs
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/cramfsck
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CRAMFS_HOSTDLFILE)
endif
