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
# The project's page is at http://www.open7x0.org
#

PNGOUT_BIN := $(HOSTUTILS_PREFIX_BIN)/pngout

PNGOUT_HOSTVERSION := 20110722
PNGOUT_HOSTFILE := pngout-$(PNGOUT_HOSTVERSION)-linux-static.tar.gz
PNGOUT_HOSTDLFILE := $(DOWNLOAD_DIR)/$(PNGOUT_HOSTFILE)

PNGOUT_HOSTPATCHES_DIR := $(PATCHES_DIR)/pngout/$(PNGOUT_HOSTVERSION)
PNGOUT_HOSTURL := http://static.jonof.id.au/dl/kenutils/$(PNGOUT_HOSTFILE)
PNGOUT_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/pngout-$(PNGOUT_HOSTVERSION)-linux-static

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
	$(TOUCH) $(STAGEFILES_DIR)/.pngout_host_unpacked

#
# install pngout
#

$(HOSTUTILS_PREFIX_BIN)/pngout: $(STAGEFILES_DIR)/.pngout_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "#!/bin/sh" > $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "$(HOSTUTILS_PREFIX_BIN)/pngout-static-$(HOST_ARCH) \$$*" >> $(HOSTUTILS_PREFIX_BIN)/pngout
	$(ECHO) "exit 0" >> $(HOSTUTILS_PREFIX_BIN)/pngout
	chmod 755 $(HOSTUTILS_PREFIX_BIN)/pngout
	$(CP) $(PNGOUT_HOSTDIR)/$(HOST_ARCH)/pngout-static $(HOSTUTILS_PREFIX_BIN)/pngout-static-$(HOST_ARCH)

.PHONY: clean-pngout-host distclean-pngout-host

clean-pngout-host:
	-$(RM) -rf $(PNGOUT_HOSTDIR)

distclean-pngout-host:
	-$(RM) -f $(STAGEFILES_DIR)/.pngout_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngout-static-$(HOST_ARCH)
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PNGOUT_HOSTDLFILE)
endif
