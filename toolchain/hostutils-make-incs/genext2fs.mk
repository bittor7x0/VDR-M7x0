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
# $Id: genext2fs.mk 401 2009-03-26 12:29:09Z andreas $
#

GENEXT2FS_BIN := $(HOSTUTILS_PREFIX_BIN)/genext2fs

GENEXT2FS_HOSTVERSION := 1.4.1
GENEXT2FS_HOSTFILE := genext2fs-$(GENEXT2FS_HOSTVERSION).tar.gz
GENEXT2FS_HOSTDLFILE := $(DOWNLOAD_DIR)/$(GENEXT2FS_HOSTFILE)

GENEXT2FS_HOSTPATCHES_DIR := $(PATCHES_DIR)/genext2fs/$(GENEXT2FS_HOSTVERSION)
GENEXT2FS_HOSTURL := http://mesh.dl.sourceforge.net/sourceforge/genext2fs/$(GENEXT2FS_HOSTFILE)
GENEXT2FS_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/genext2fs-$(GENEXT2FS_HOSTVERSION)

CLEAN_RULES += clean-genext2fs-host
DISTCLEAN_RULES += distclean-genext2fs-host

#
# download genext2fs
#

$(GENEXT2FS_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GENEXT2FS_HOSTDLFILE) ] ; then \
	$(WGET) $(GENEXT2FS_HOSTURL) -O $(GENEXT2FS_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(GENEXT2FS_HOSTDLFILE)

#
# unpack genext2fs
#

$(STAGEFILES_DIR)/.genext2fs_host_unpacked: $(GENEXT2FS_HOSTDLFILE) \
                                         $(wildcard $(GENEXT2FS_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(GENEXT2FS_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(GENEXT2FS_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.genext2fs_host_unpacked

#
# patch genext2fs
#

$(STAGEFILES_DIR)/.genext2fs_host_patched: $(STAGEFILES_DIR)/.genext2fs_host_unpacked
	$(call patch_package, $(GENEXT2FS_HOSTDIR), $(GENEXT2FS_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.genext2fs_host_patched


$(STAGEFILES_DIR)/.genext2fs_host_configured: $(STAGEFILES_DIR)/.genext2fs_host_patched
	-$(MAKE) -C $(GENEXT2FS_HOSTDIR) distclean
	($(CD) $(GENEXT2FS_HOSTDIR) ; \
		$(GENEXT2FS_HOSTDIR)/configure \
			--prefix=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.genext2fs_host_configured

#
# compile genext2fs
#

$(STAGEFILES_DIR)/.genext2fs_host_compiled: $(STAGEFILES_DIR)/.genext2fs_host_configured
	-$(MAKE) -C $(GENEXT2FS_HOSTDIR) clean
	$(MAKE) -C $(GENEXT2FS_HOSTDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.genext2fs_host_compiled

$(STAGEFILES_DIR)/.genext2fs_host_installed: $(STAGEFILES_DIR)/.genext2fs_host_compiled
	$(MAKE) -C $(GENEXT2FS_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.genext2fs_host_installed

#
# install genext2fs
#

$(HOSTUTILS_PREFIX_BIN)/genext2fs: $(STAGEFILES_DIR)/.genext2fs_host_installed
	$(TOUCH) $(HOSTUTILS_PREFIX_BIN)/genext2fs



.PHONY: clean-genext2fs-host distclean-genext2fs-host

clean-genext2fs-host:
	-$(RM) -rf $(GENEXT2FS_HOSTDIR)

distclean-genext2fs-host:
	-$(RM) -f $(STAGEFILES_DIR)/.genext2fs_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.genext2fs_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.genext2fs_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.genext2fs_host_installed
	-$(RM) -f $(STAGEFILES_DIR)/.genext2fs_host_configured
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/genext2fs
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GENEXT2FS_HOSTDLFILE)
endif
