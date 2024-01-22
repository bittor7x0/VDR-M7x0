# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2019-2024 VDR-NG-EM Project
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

PNGWOLF-ZOPFLI_BIN := $(HOSTUTILS_PREFIX_BIN)/pngwolf-zopfli

PNGWOLF-ZOPFLI_HOSTVERSION := 1.1.2

PNGWOLF-ZOPFLI_HOSTPATCHES_DIR := $(PATCHES_DIR)/pngwolf-zopfli/$(PNGWOLF-ZOPFLI_HOSTVERSION)
PNGWOLF-ZOPFLI_HOSTURL := https://github.com/jibsen/pngwolf-zopfli
PNGWOLF-ZOPFLI_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/pngwolf-zopfli-$(PNGWOLF-ZOPFLI_HOSTVERSION)

CLEAN_RULES += clean-pngwolf-zopfli-host
DISTCLEAN_RULES += distclean-pngwolf-zopfli-host

#
# download pngwolf-zopfli
#

$(STAGEFILES_DIR)/.pngwolf-zopfli_host_downloaded: $(TC_INIT_RULE)
	-$(RM) -rf $(PNGWOLF-ZOPFLI_HOSTDIR)
	$(GIT) clone -b 'v$(PNGWOLF-ZOPFLI_HOSTVERSION)' --single-branch --depth 1 $(PNGWOLF-ZOPFLI_HOSTURL) $(PNGWOLF-ZOPFLI_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.pngwolf-zopfli_host_downloaded

#
# unpack pngwolf-zopfli
#

$(STAGEFILES_DIR)/.pngwolf-zopfli_host_prepared: $(STAGEFILES_DIR)/.pngwolf-zopfli_host_downloaded \
                                           $(wildcard $(PNGWOLF-ZOPFLI_HOSTPATCHES_DIR)/*.patch) \
                                           $$(PNGWOLF-ZOPFLI_HOSTDEPS)
	(cd $(PNGWOLF-ZOPFLI_HOSTDIR) ; git submodule update --init --recursive --recommend-shallow)
	$(TOUCH) $(STAGEFILES_DIR)/.pngwolf-zopfli_host_prepared

#
# patch pngwolf-zopfli
#

$(STAGEFILES_DIR)/.pngwolf-zopfli_host_patched: $(STAGEFILES_DIR)/.pngwolf-zopfli_host_prepared
	$(call patch_package, $(PNGWOLF-ZOPFLI_HOSTDIR), $(PNGWOLF-ZOPFLI_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.pngwolf-zopfli_host_patched

#
# compile pngwolf-zopfli
#

$(STAGEFILES_DIR)/.pngwolf-zopfli_host_compiled: $(STAGEFILES_DIR)/.pngwolf-zopfli_host_patched
	-$(MAKE) -C $(PNGWOLF-ZOPFLI_HOSTDIR)/src clean
	$(MAKE) -C $(PNGWOLF-ZOPFLI_HOSTDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.pngwolf-zopfli_host_compiled

#
# install pngwolf-zopfli
#

$(HOSTUTILS_PREFIX_BIN)/pngwolf-zopfli: $(STAGEFILES_DIR)/.pngwolf-zopfli_host_compiled
	$(CP) $(PNGWOLF-ZOPFLI_HOSTDIR)/pngwolf $(HOSTUTILS_PREFIX_BIN)/pngwolf-zopfli

.PHONY: clean-pngwolf-zopfli-host distclean-pngwolf-zopfli-host

clean-pngwolf-zopfli-host:
	-$(RM) -rf $(PNGWOLF-ZOPFLI_HOSTDIR)
	-$(RM) -f $(STAGEFILES_DIR)/.pngwolf-zopfli_host_downloaded

distclean-pngwolf-zopfli-host:
	-$(RM) -f $(STAGEFILES_DIR)/.pngwolf-zopfli_host_prepared
	-$(RM) -f $(STAGEFILES_DIR)/.pngwolf-zopfli_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.pngwolf-zopfli_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/pngwolf-zopfli
