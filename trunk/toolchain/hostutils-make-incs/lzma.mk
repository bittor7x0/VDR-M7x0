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

LZMA_BIN := $(HOSTUTILS_PREFIX_BIN)/lzma

LZMA_HOSTVERSION := 443
LZMA_HOSTFILE := lzma$(LZMA_HOSTVERSION).tar.bz2
LZMA_HOSTDLFILE := $(DOWNLOAD_DIR)/$(LZMA_HOSTFILE)

LZMA_HOSTPATCHES_DIR := $(PATCHES_DIR)/lzma/$(LZMA_HOSTVERSION)
LZMA_HOSTURL := http://kent.dl.sourceforge.net/sourceforge/sevenzip/$(LZMA_HOSTFILE)
LZMA_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/lzma$(LZMA_HOSTVERSION)
LZMA_HOSTLIBDIR := $(LZMA_HOSTDIR)/C/7zip/Compress/LZMA_Lib
LZMA_HOSTALONEDIR := $(LZMA_HOSTDIR)/C/7zip/Compress/LZMA_Alone

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
	$(MKDIR) -p $(LZMA_HOSTDIR)
	$(BZCAT) $(LZMA_HOSTDLFILE) | $(TAR) -C $(LZMA_HOSTDIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_unpacked

#
# patch lzma
#

$(STAGEFILES_DIR)/.lzma_host_patched: $(STAGEFILES_DIR)/.lzma_host_unpacked
	$(call patch_package, $(LZMA_HOSTDIR), $(LZMA_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_patched

#
# compile lzma
#

$(STAGEFILES_DIR)/.lzma_host_compiled: $(STAGEFILES_DIR)/.lzma_host_patched
	$(MAKE) -C $(LZMA_HOSTLIBDIR) clean
	$(MAKE) -C $(LZMA_HOSTLIBDIR) all
	$(MAKE) -f makefile.gcc -C $(LZMA_HOSTALONEDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.lzma_host_compiled

$(STAGEFILES_DIR)/.lzma_host_installed: $(STAGEFILES_DIR)/.lzma_host_compiled
	$(CP) $(LZMA_HOSTLIBDIR)/liblzma.a $(HOSTUTILS_PREFIX)/lib/liblzma.a
	$(CP) $(LZMA_HOSTALONEDIR)/lzma $(HOSTUTILS_PREFIX_BIN)/lzma
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
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/lzma
	-$(RM) -f $(HOSTUTILS_PREFIX)/lib/liblzma.a
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LZMA_HOSTDLFILE)
endif
