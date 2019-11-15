# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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

PINGO_BIN := $(HOSTUTILS_PREFIX_BIN)/pingo

PINGO_HOSTVERSION := 0.99.beta63
PINGO_HOSTFILE := pingo-nix64.zip
PINGO_HOSTDLFILE := $(DOWNLOAD_DIR)/$(PINGO_HOSTFILE)

PINGO_HOSTURL := https://css-ig.net/bin/$(PINGO_HOSTFILE)
PINGO_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/pingo-$(PINGO_HOSTVERSION)

CLEAN_RULES += clean-pingo-host
DISTCLEAN_RULES += distclean-pingo-host

#
# download pingo
#

$(PINGO_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PINGO_HOSTDLFILE) ] ; then \
	$(WGET) $(PINGO_HOSTURL) -O $(PINGO_HOSTDLFILE) \
		--header='authority: css-ig.net' \
		--header='upgrade-insecure-requests: 1' \
		--header='User-Agent: Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8' \
		--header='Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3' \
		--header='Referer: https://css-ig.net/pingo' \
		--header='Accept-Encoding: gzip, deflate, br' \
		--header='Accept-Language: en-US,en;q=0.9,pt;q=0.8' \
		--header='Cookie: HttpOnly; SERVERID105614=1420122|XSyhV|XSyhV' ; \
	fi );
	$(TOUCH) $(PINGO_HOSTDLFILE)

#
# unpack pingo
#

$(STAGEFILES_DIR)/.pingo_host_unpacked: $(PINGO_HOSTDLFILE)
	-$(RM) -rf $(PINGO_HOSTDIR)
	$(MKDIR) -p $(PINGO_HOSTDIR)
	(cd $(PINGO_HOSTDIR) ; $(UNZIP) -x $(PINGO_HOSTDLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.pingo_host_unpacked

#
# install pingo
#

$(HOSTUTILS_PREFIX_BIN)/pingo: $(STAGEFILES_DIR)/.pingo_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pingo
	chmod 755 $(PINGO_HOSTDIR)/pingo
	$(CP) $(PINGO_HOSTDIR)/pingo $(HOSTUTILS_PREFIX_BIN)/pingo

.PHONY: clean-pingo-host distclean-pingo-host

clean-pingo-host:
	-$(RM) -rf $(PINGO_HOSTDIR)

distclean-pingo-host:
	-$(RM) -f $(STAGEFILES_DIR)/.pingo_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pingo
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PINGO_HOSTDLFILE)
endif
