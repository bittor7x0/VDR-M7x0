# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2016 VDR-NG-EM Project
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

ZOPFLIPNG_BIN := $(HOSTUTILS_PREFIX_BIN)/zopflipng

ZOPFLIPNG_HOSTVERSION := ae43a8b73827577c4b19b005b6eed81f5cf9bbac
ZOPFLIPNG_HOSTFILE := zopfli-$(ZOPFLIPNG_HOSTVERSION).zip
ZOPFLIPNG_HOSTDLFILE := $(DOWNLOAD_DIR)/$(ZOPFLIPNG_HOSTFILE)

ZOPFLIPNG_HOSTPATCHES_DIR := $(PATCHES_DIR)/zopflipng/snapshot
ZOPFLIPNG_HOSTURL := https://github.com/google/zopfli/archive/$(ZOPFLIPNG_HOSTVERSION).zip
ZOPFLIPNG_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/zopfli-$(ZOPFLIPNG_HOSTVERSION)

CLEAN_RULES += clean-zopflipng-host
DISTCLEAN_RULES += distclean-zopflipng-host

#
# download zopflipng
#

$(ZOPFLIPNG_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(ZOPFLIPNG_HOSTDLFILE) ] ; then \
	$(WGET) $(ZOPFLIPNG_HOSTURL) -O $(ZOPFLIPNG_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(ZOPFLIPNG_HOSTDLFILE)

#
# unpack zopflipng
#

$(STAGEFILES_DIR)/.zopflipng_host_unpacked: $(ZOPFLIPNG_HOSTDLFILE) \
                                         $(wildcard $(ZOPFLIPNG_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(ZOPFLIPNG_HOSTDIR)
	(cd $(HOSTUTILS_BUILD_DIR) ; $(UNZIP) -x $(ZOPFLIPNG_HOSTDLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.zopflipng_host_unpacked

#
# patch zopflipng
#

$(STAGEFILES_DIR)/.zopflipng_host_patched: $(STAGEFILES_DIR)/.zopflipng_host_unpacked
	$(call patch_package, $(ZOPFLIPNG_HOSTDIR), $(ZOPFLIPNG_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.zopflipng_host_patched

#
# compile zopflipng
#

$(STAGEFILES_DIR)/.zopflipng_host_compiled: $(STAGEFILES_DIR)/.zopflipng_host_patched
	$(MAKE) -C $(ZOPFLIPNG_HOSTDIR) zopflipng
	$(TOUCH) $(STAGEFILES_DIR)/.zopflipng_host_compiled

#
# install zopflipng
#

$(HOSTUTILS_PREFIX_BIN)/zopflipng: $(STAGEFILES_DIR)/.zopflipng_host_compiled
	$(CP) $(ZOPFLIPNG_HOSTDIR)/zopflipng $(HOSTUTILS_PREFIX_BIN)/zopflipng

.PHONY: clean-zopflipng-host distclean-zopflipng-host

clean-zopflipng-host:
	-$(RM) -rf $(ZOPFLIPNG_HOSTDIR)

distclean-zopflipng-host:
	-$(RM) -f $(STAGEFILES_DIR)/.zopflipng_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.zopflipng_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.zopflipng_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/zopflipng
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(ZOPFLIPNG_HOSTDLFILE)
endif
