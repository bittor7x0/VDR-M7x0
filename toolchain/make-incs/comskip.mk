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

ifeq ($(CONFIG_COMSKIP),y)
ifneq ($(and $(filter y,$(CONFIG_ARGTABLE2)),$(filter y,$(CONFIG_FFMPEG)),$(filter y,$(CONFIG_FFMPEG_LIBAVCODEC)),$(filter y,$(CONFIG_FFMPEG_LIBAVUTIL)),$(filter y,$(CONFIG_FFMPEG_LIBAVFORMAT))),y)
   $(error dependency error: comskip needs argtable2 and ffmpeg with libavcodec, libavutil and libavformat libraries enabled)
endif
endif 

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
COMSKIP_DEPS = $(BASE_BUILD_STAGEFILE) $(ARGTABLE2_INSTALLED) $(FFMPEG_INSTALLED)

COMSKIP_VERSION := dc0712882ed6cc6be6a2ff503054889a8760f793
COMSKIP_PATCHES_DIR := $(PATCHES_DIR)/comskip/snapshot

COMSKIP_FILE := Comskip-$(COMSKIP_VERSION).zip
COMSKIP_DLFILE := $(DOWNLOAD_DIR)/$(COMSKIP_FILE)
COMSKIP_URL := https://github.com/erikkaashoek/Comskip/archive/$(COMSKIP_VERSION).zip
COMSKIP_DIR := $(BUILD_DIR)/Comskip-$(COMSKIP_VERSION)

COMSKIP_INSTALLED = $(STAGEFILES_DIR)/.comskip_installed

PACKS_RULES_$(CONFIG_COMSKIP) += $(COMSKIP_INSTALLED)
FILE_LISTS_$(CONFIG_COMSKIP) += comskip.lst

CLEAN_RULES += clean-comskip
DISTCLEAN_RULES += distclean-comskip

#
# download comskip
#

$(COMSKIP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(COMSKIP_DLFILE) ] ; then \
	$(WGET) $(COMSKIP_URL) -O $(COMSKIP_DLFILE) ; \
	fi );
	$(TOUCH) $(COMSKIP_DLFILE)

#
# unpack comskip
#

$(STAGEFILES_DIR)/.comskip_unpacked: $(COMSKIP_DLFILE) \
                                           $(wildcard $(COMSKIP_PATCHES_DIR)/*.patch) \
                                           $$(COMSKIP_DEPS)
	-$(RM) -rf $(COMSKIP_DIR)
	(cd $(BUILD_DIR) ; $(UNZIP) -x $(COMSKIP_DLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.comskip_unpacked

#
# patch comskip
#

$(STAGEFILES_DIR)/.comskip_patched: $(STAGEFILES_DIR)/.comskip_unpacked
	$(call patch_package, $(COMSKIP_DIR), $(COMSKIP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.comskip_patched

#
# configure comskip
#

$(STAGEFILES_DIR)/.comskip_configured: $(STAGEFILES_DIR)/.comskip_patched
	($(CD) $(COMSKIP_DIR) ; $(COMSKIP_DIR)/autogen.sh ; \
		$(UCLIBC_ENV_LTO_GC) \
		LDFLAGS="$(UCLIBC_LDFLAGS_LTO) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(COMSKIP_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-donator)
	$(TOUCH) $(STAGEFILES_DIR)/.comskip_configured

#
# compile comskip
#

$(STAGEFILES_DIR)/.comskip_compiled: $(STAGEFILES_DIR)/.comskip_configured
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(COMSKIP_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.comskip_compiled

#
# pack comskip
#

$(STAGEFILES_DIR)/.comskip_installed: $(STAGEFILES_DIR)/.comskip_compiled
	$(CP) $(COMSKIP_DIR)/comskip $(TARGET_ROOT)/usr/bin/comskip
	$(TOUCH) $(STAGEFILES_DIR)/.comskip_installed


$(FILELIST_DIR)/comskip.lst: $(STAGEFILES_DIR)/.comskip_installed
	$(TOUCH) $(FILELIST_DIR)/comskip.lst

.PHONY: clean-comskip distclean-comskip

clean-comskip:
	-$(RM) -rf $(COMSKIP_DIR)

#
# clean everthing else
#

distclean-comskip:
	-$(RM) -f $(STAGEFILES_DIR)/.comskip_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.comskip_patched
	-$(RM) -f $(STAGEFILES_DIR)/.comskip_configured
	-$(RM) -f $(STAGEFILES_DIR)/.comskip_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.comskip_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(COMSKIP_DLFILE)
endif
