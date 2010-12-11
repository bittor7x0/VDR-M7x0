# Copyright (C) 2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id: squashfs.mk 401 2009-03-26 12:29:09Z andreas $
#

ifeq ($(CONFIG_SQUASHFS_LZMA),y)
MKSQUASHFS := mksquashfs-lzma
else
MKSQUASHFS := mksquashfs
endif

MKSQUASHFS_BIN := $(HOSTUTILS_PREFIX_BIN)/$(MKSQUASHFS)

SQUASHFS_HOSTVERSION := 3.1-r2
SQUASHFS_HOSTFILE := squashfs$(SQUASHFS_HOSTVERSION).tar.gz
SQUASHFS_HOSTDLFILE := $(DOWNLOAD_DIR)/$(SQUASHFS_HOSTFILE)

SQUASHFS_HOSTPATCHES_DIR := $(PATCHES_DIR)/squashfs/$(SQUASHFS_HOSTVERSION)
SQUASHFS_HOSTURL := http://mesh.dl.sourceforge.net/sourceforge/squashfs/$(SQUASHFS_HOSTFILE)
SQUASHFS_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/squashfs$(SQUASHFS_HOSTVERSION)

CLEAN_RULES += clean-squashfs-host
DISTCLEAN_RULES += distclean-squashfs-host

#
# download squashfs
#

$(SQUASHFS_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SQUASHFS_HOSTDLFILE) ] ; then \
	$(WGET) $(SQUASHFS_HOSTURL) -O $(SQUASHFS_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(SQUASHFS_HOSTDLFILE)

#
# unpack squashfs
#

$(STAGEFILES_DIR)/.squashfs_host_unpacked: $(SQUASHFS_HOSTDLFILE) \
                                         $(wildcard $(SQUASHFS_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(SQUASHFS_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(SQUASHFS_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.squashfs_host_unpacked

#
# patch squashfs
#

$(STAGEFILES_DIR)/.squashfs_host_patched: $(STAGEFILES_DIR)/.squashfs_host_unpacked
	$(call patch_package, $(SQUASHFS_HOSTDIR), $(SQUASHFS_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.squashfs_host_patched

#
# compile squashfs
#

$(STAGEFILES_DIR)/.squashfs_host_compiled: $(STAGEFILES_DIR)/.squashfs_host_patched
	$(MAKE) -C $(SQUASHFS_HOSTDIR)/squashfs-tools clean
	$(MAKE) -C $(SQUASHFS_HOSTDIR)/squashfs-tools LZMAPATH=$(LZMA_HOSTLIBDIR) $(MKSQUASHFS)
	$(TOUCH) $(STAGEFILES_DIR)/.squashfs_host_compiled

#
# install squashfs
#

$(MKSQUASHFS_BIN): $(STAGEFILES_DIR)/.squashfs_host_compiled
	$(CP) $(SQUASHFS_HOSTDIR)/squashfs-tools/$(MKSQUASHFS) $(MKSQUASHFS_BIN)

.PHONY: clean-squashfs-host distclean-squashfs-host

clean-squashfs-host:
	-$(RM) -rf $(SQUASHFS_HOSTDIR)

distclean-squashfs-host:
	-$(RM) -f $(STAGEFILES_DIR)/.squashfs_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.squashfs_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.squashfs_host_compiled
	-$(RM) -f $(MKSQUASHFS_BIN)
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SQUASHFS_HOSTDLFILE)
endif
