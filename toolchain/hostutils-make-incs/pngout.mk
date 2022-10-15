# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2022 VDR-NG-EM Project
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

PNGOUT_BIN := $(HOSTUTILS_PREFIX_BIN)/pngout

PNGOUT_HOSTVERSION := 20200115
PNGOUT_HOSTFILE := pngout-$(PNGOUT_HOSTVERSION)-linux.tar.gz
PNGOUT_HOSTDLFILE := $(DOWNLOAD_DIR)/$(PNGOUT_HOSTFILE)

PNGOUT_HOSTURL := http://www.jonof.id.au/files/kenutils/$(PNGOUT_HOSTFILE)
PNGOUT_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/pngout-$(PNGOUT_HOSTVERSION)-linux

CLEAN_RULES += clean-pngout-host
DISTCLEAN_RULES += distclean-pngout-host

#
# download pngout
#

$(PNGOUT_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PNGOUT_HOSTDLFILE) ] ; then \
	$(WGET) $(PNGOUT_HOSTURL) -O $(PNGOUT_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(PNGOUT_HOSTDLFILE)

#
# unpack pngout
#

$(STAGEFILES_DIR)/.pngout_host_unpacked: $(PNGOUT_HOSTDLFILE)
	-$(RM) -rf $(PNGOUT_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(PNGOUT_HOSTDLFILE)
	$(MV) $(PNGOUT_HOSTDIR)/amd64 $(PNGOUT_HOSTDIR)/x86_64
	$(TOUCH) $(STAGEFILES_DIR)/.pngout_host_unpacked

#
# install pngout
#

$(HOSTUTILS_PREFIX_BIN)/pngout: $(STAGEFILES_DIR)/.pngout_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "#!/bin/sh" > $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "$(HOSTUTILS_PREFIX_BIN)/pngout-$(HOST_ARCH) \$$*" >> $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "exit 0" >> $(HOSTUTILS_PREFIX_BIN)/pngout
	chmod 755 $(HOSTUTILS_PREFIX_BIN)/pngout
	$(CP) $(PNGOUT_HOSTDIR)/$(HOST_ARCH)/pngout $(HOSTUTILS_PREFIX_BIN)/pngout-$(HOST_ARCH)

.PHONY: clean-pngout-host distclean-pngout-host

clean-pngout-host:
	-$(RM) -rf $(PNGOUT_HOSTDIR)

distclean-pngout-host:
	-$(RM) -f $(STAGEFILES_DIR)/.pngout_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout-$(HOST_ARCH)
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PNGOUT_HOSTDLFILE)
endif
