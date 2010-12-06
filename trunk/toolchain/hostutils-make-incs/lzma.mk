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
# $Id: lzma.mk 401 2009-03-26 12:29:09Z andreas $
#

LZMA_BIN := $(HOSTUTILS_PREFIX_BIN)/lzma

LZMA_HOSTVERSION := 4.32.7
LZMA_HOSTFILE := lzma-$(LZMA_HOSTVERSION).tar.gz
LZMA_HOSTDLFILE := $(DOWNLOAD_DIR)/$(LZMA_HOSTFILE)

LZMA_HOSTPATCHES_DIR := $(PATCHES_DIR)/lzma/$(LZMA_HOSTVERSION)
LZMA_HOSTURL := http://tukaani.org/lzma/$(LZMA_HOSTFILE)
LZMA_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/lzma-$(LZMA_HOSTVERSION)

CLEAN_RULES += clean-lzma-host
DISTCLEAN_RULES += distclean-lzma-host

#
# download lzma
#

$(LZMA_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LZMA_HOSTDLFILE) ] ; then \
	$(WGET) $(LZMA_HOSTURL) -O $(LZMA_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(LZMA_HOSTDLFILE)

#
# unpack lzma
#

$(STAGEFILES_DIR)/.lzma_host_unpacked: $(LZMA_HOSTDLFILE) \
                                         $(wildcard $(LZMA_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(LZMA_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(LZMA_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_unpacked

#
# patch lzma
#

$(STAGEFILES_DIR)/.lzma_host_patched: $(STAGEFILES_DIR)/.lzma_host_unpacked
	$(call patch_package, $(LZMA_HOSTDIR), $(LZMA_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_patched


$(STAGEFILES_DIR)/.lzma_host_configured: $(STAGEFILES_DIR)/.lzma_host_patched
	-$(MAKE) -C $(LZMA_HOSTDIR) distclean
	($(CD) $(LZMA_HOSTDIR) ; \
		$(LZMA_HOSTDIR)/configure --disable-shared \
			--prefix=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_configured

#
# compile lzma
#

$(STAGEFILES_DIR)/.lzma_host_compiled: $(STAGEFILES_DIR)/.lzma_host_configured
	-$(MAKE) -C $(LZMA_HOSTDIR) clean
	$(MAKE) -C $(LZMA_HOSTDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_compiled

$(STAGEFILES_DIR)/.lzma_host_installed: $(STAGEFILES_DIR)/.lzma_host_compiled
	$(MAKE) -C $(LZMA_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_installed

#
# install lzma
#

$(HOSTUTILS_PREFIX_BIN)/lzma: $(STAGEFILES_DIR)/.lzma_host_installed
	$(TOUCH) $(HOSTUTILS_PREFIX_BIN)/lzma



.PHONY: clean-lzma-host distclean-lzma-host

clean-lzma-host:
	-$(RM) -rf $(LZMA_HOSTDIR)

distclean-lzma-host:
	-$(RM) -f $(STAGEFILES_DIR)/.lzma_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.lzma_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.lzma_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.lzma_host_installed
	-$(RM) -f $(STAGEFILES_DIR)/.lzma_host_configured
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/lzma
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LZMA_HOSTDLFILE)
endif
