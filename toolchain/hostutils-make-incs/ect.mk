# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2019-2022 VDR-NG-EM Project
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

# Put dependencies here
ECT_HOSTDEPS = $(NASM_HOSTINSTALLED)

ECT_BIN := $(HOSTUTILS_PREFIX_BIN)/ect

ECT_HOSTVERSION := 0.8.3

ECT_HOSTPATCHES_DIR := $(PATCHES_DIR)/ect/$(ECT_HOSTVERSION)
ECT_HOSTURL := https://github.com/fhanau/Efficient-Compression-Tool
ECT_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/Efficient-Compression-Tool-$(ECT_HOSTVERSION)

CLEAN_RULES += clean-ect-host
DISTCLEAN_RULES += distclean-ect-host

#
# download ect
#

$(STAGEFILES_DIR)/.ect_host_downloaded: $(TC_INIT_RULE)
	-$(RM) -rf $(ECT_HOSTDIR)
	$(GIT) clone -b 'v$(ECT_HOSTVERSION)' --single-branch --depth 1 $(ECT_HOSTURL) $(ECT_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ect_host_downloaded

#
# unpack ect
#

$(STAGEFILES_DIR)/.ect_host_prepared: $(STAGEFILES_DIR)/.ect_host_downloaded \
                                           $(wildcard $(ECT_HOSTPATCHES_DIR)/*.patch) \
                                           $$(ECT_HOSTDEPS)
	(cd $(ECT_HOSTDIR)/src ; git submodule update --init --recursive --recommend-shallow ; cd mozjpeg ; ASM_NASM=$(PREFIX_BIN)/nasm cmake -G"Unix Makefiles")
	$(TOUCH) $(STAGEFILES_DIR)/.ect_host_prepared

#
# patch ect
#

$(STAGEFILES_DIR)/.ect_host_patched: $(STAGEFILES_DIR)/.ect_host_prepared
	$(call patch_package, $(ECT_HOSTDIR), $(ECT_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ect_host_patched

#
# compile ect
#

$(STAGEFILES_DIR)/.ect_host_compiled: $(STAGEFILES_DIR)/.ect_host_patched
	-$(MAKE) -C $(ECT_HOSTDIR)/src clean
	NASM=$(PREFIX_BIN)/nasm $(MAKE) -C $(ECT_HOSTDIR)/src all
	$(TOUCH) $(STAGEFILES_DIR)/.ect_host_compiled

#
# install ect
#

$(HOSTUTILS_PREFIX_BIN)/ect: $(STAGEFILES_DIR)/.ect_host_compiled
	$(CP) $(ECT_HOSTDIR)/ect $(HOSTUTILS_PREFIX_BIN)/ect

.PHONY: clean-ect-host distclean-ect-host

clean-ect-host:
	-$(RM) -rf $(ECT_HOSTDIR)
	-$(RM) -f $(STAGEFILES_DIR)/.ect_host_downloaded

distclean-ect-host:
	-$(RM) -f $(STAGEFILES_DIR)/.ect_host_prepared
	-$(RM) -f $(STAGEFILES_DIR)/.ect_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ect_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/ect
