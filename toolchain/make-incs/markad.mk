# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2013-2021 VDR-NG-EM Project
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

ifeq ($(CONFIG_MARKAD),y)
ifneq ($(and $(filter y,$(CONFIG_LIBINTL-LITE)),$(filter y,$(CONFIG_FFMPEG)),$(filter y,$(CONFIG_FFMPEG_LIBAVCODEC)),$(filter y,$(CONFIG_FFMPEG_LIBAVUTIL)),$(filter y,$(CONFIG_FFMPEG_LIBAVFORMAT))),y)
   $(error dependency error: markad needs libintl-lite and ffmpeg with libavcodec, libavutil and libavformat libraries enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
MARKAD_DEPS = $(BASE_BUILD_STAGEFILE) $(FFMPEG_INSTALLED) $(LIBINTL-LITE_INSTALLED)

MARKAD_VERSION := 74e2a8c5382fa8bfacd12274899112724a1e0d51
MARKAD_PATCHES_DIR := $(PATCHES_DIR)/markad/snapshot

MARKAD_FILE := vdr-plugin-markad-$(MARKAD_VERSION).tar.bz2
MARKAD_DLFILE := $(DOWNLOAD_DIR)/$(MARKAD_FILE)
MARKAD_URL := https://projects.vdr-developer.org/git/vdr-plugin-markad.git/snapshot/$(MARKAD_FILE)
MARKAD_DIR := $(BUILD_DIR)/vdr-plugin-markad-$(MARKAD_VERSION)

MARKAD_INSTALLED = $(STAGEFILES_DIR)/.markad_installed

PACKS_RULES_$(CONFIG_MARKAD) += $(MARKAD_INSTALLED)
FILE_LISTS_$(CONFIG_MARKAD) += markad.lst

CLEAN_RULES += clean-markad
DISTCLEAN_RULES += distclean-markad

#
# download markad
#

$(MARKAD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MARKAD_DLFILE) ] ; then \
	$(WGET) $(MARKAD_URL) -O $(MARKAD_DLFILE) ; \
	fi );
	$(TOUCH) $(MARKAD_DLFILE)

#
# unpack markad
#

$(STAGEFILES_DIR)/.markad_unpacked: $(MARKAD_DLFILE) \
                                           $(wildcard $(MARKAD_PATCHES_DIR)/*.patch) \
                                           $$(MARKAD_DEPS)
	-$(RM) -rf $(MARKAD_DIR)
	$(BZCAT) $(MARKAD_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.markad_unpacked

#
# patch markad
#

$(STAGEFILES_DIR)/.markad_patched: $(STAGEFILES_DIR)/.markad_unpacked
	$(call patch_package, $(MARKAD_DIR), $(MARKAD_PATCHES_DIR))
	(for file in `$(FIND) $(MARKAD_DIR) -type f -iname '*' | $(SORT)` ; do \
		$(SED) -i -e  "s,var\/lib\/markad,var\/vdr\/video0,g" $$file ; \
	done)
	$(TOUCH) $(STAGEFILES_DIR)/.markad_patched

#
# compile markad
#

$(STAGEFILES_DIR)/.markad_compiled: $(STAGEFILES_DIR)/.markad_patched
	$(UCLIBC_ENV) \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		CFLAGS="$(UCLIBC_CFLAGS_LTO_GC) $(UCLIBC_CFLAGS_LOOPS)" \
		$(MAKE) -C $(MARKAD_DIR)/command \
		PKG-CONFIG="pkg-config" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		DESTDIR="$(TARGET_ROOT)"
	$(TOUCH) $(STAGEFILES_DIR)/.markad_compiled

#
# install markad
#

$(STAGEFILES_DIR)/.markad_installed: $(STAGEFILES_DIR)/.markad_compiled
	$(UCLIBC_ENV) \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		CFLAGS="$(UCLIBC_CFLAGS_LTO_GC) $(UCLIBC_CFLAGS_LOOPS)" \
		$(MAKE) -C $(MARKAD_DIR)/command install \
		PKG-CONFIG="pkg-config" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		DESTDIR="$(TARGET_ROOT)"
	$(TOUCH) $(STAGEFILES_DIR)/.markad_installed


$(FILELIST_DIR)/markad.lst: $(STAGEFILES_DIR)/.markad_installed
	$(TOUCH) $(FILELIST_DIR)/markad.lst

.PHONY: clean-markad distclean-markad

clean-markad:
	-$(RM) -rf $(MARKAD_DIR)

#
# clean everthing else
#

distclean-markad:
	-$(RM) -f $(STAGEFILES_DIR)/.markad_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.markad_patched
	-$(RM) -f $(STAGEFILES_DIR)/.markad_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.markad_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MARKAD_DLFILE)
endif
