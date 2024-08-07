# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2024 VDR-NG-EM Project
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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
GRAPHICSMAGICK_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_LIBPNG),y)
   GRAPHICSMAGICK_DEPS += $(LIBPNG_INSTALLED)
endif

ifeq ($(CONFIG_LIBJPEG),y)
   GRAPHICSMAGICK_DEPS += $(LIBJPEG_INSTALLED)
endif

ifeq ($(CONFIG_FREETYPE),y)
   GRAPHICSMAGICK_DEPS += $(FREETYPE_INSTALLED)
endif

ifeq ($(CONFIG_ZLIB),y)
   GRAPHICSMAGICK_DEPS += $(ZLIB_INSTALLED)
endif

GRAPHICSMAGICK_VERSION := 1.3.24
GRAPHICSMAGICK_PATCHES_DIR := $(PATCHES_DIR)/graphicsmagick/$(GRAPHICSMAGICK_VERSION)

GRAPHICSMAGICK_FILE := GraphicsMagick-$(GRAPHICSMAGICK_VERSION).tar.bz2
GRAPHICSMAGICK_DLFILE := $(DOWNLOAD_DIR)/$(GRAPHICSMAGICK_FILE)
GRAPHICSMAGICK_URL := http://sourceforge.net/projects/graphicsmagick/files/graphicsmagick/$(GRAPHICSMAGICK_VERSION)/$(GRAPHICSMAGICK_FILE)/download
GRAPHICSMAGICK_DIR := $(BUILD_DIR)/GraphicsMagick-$(GRAPHICSMAGICK_VERSION)

GRAPHICSMAGICK_INSTALLED = $(STAGEFILES_DIR)/.graphicsmagick_installed

PACKS_RULES_$(CONFIG_GRAPHICSMAGICK) += $(GRAPHICSMAGICK_INSTALLED)
FILE_LISTS_$(CONFIG_GRAPHICSMAGICK) += graphicsmagick.lst

CLEAN_RULES += clean-graphicsmagick
DISTCLEAN_RULES += distclean-graphicsmagick

#
# download graphicsmagick
#

$(GRAPHICSMAGICK_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GRAPHICSMAGICK_DLFILE) ] ; then \
	$(WGET) $(GRAPHICSMAGICK_URL) -O $(GRAPHICSMAGICK_DLFILE) ; \
	fi );
	$(TOUCH) $(GRAPHICSMAGICK_DLFILE)

#
# unpack graphicsmagick
#

$(STAGEFILES_DIR)/.graphicsmagick_unpacked: $(GRAPHICSMAGICK_DLFILE) \
                                           $(wildcard $(GRAPHICSMAGICK_PATCHES_DIR)/*.patch) \
                                           $$(GRAPHICSMAGICK_DEPS)
	-$(RM) -rf $(GRAPHICSMAGICK_DIR)
	$(BZCAT) $(GRAPHICSMAGICK_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.graphicsmagick_unpacked

#
# patch graphicsmagick
#

$(STAGEFILES_DIR)/.graphicsmagick_patched: $(STAGEFILES_DIR)/.graphicsmagick_unpacked
	$(call patch_package, $(GRAPHICSMAGICK_DIR), $(GRAPHICSMAGICK_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.graphicsmagick_patched

#
# configure graphicsmagick
#

$(STAGEFILES_DIR)/.graphicsmagick_configured: $(STAGEFILES_DIR)/.graphicsmagick_patched
	($(CD) $(GRAPHICSMAGICK_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		$(GRAPHICSMAGICK_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--disable-openmp \
			$(if $(CONFIG_LIBPNG),--with-png,--without-png) \
			$(if $(CONFIG_LIBJPEG),--with-jpeg,--without-jpeg) \
			$(if $(CONFIG_FREETYPE),--with-ttf,--without-ttf) \
			$(if $(CONFIG_ZLIB),--with-zlib,--without-zlib) \
			--without-bzlib \
			--without-dps \
			--without-jbig \
			--without-webp \
			--without-jp2 \
			--without-lcms2 \
			--without-lzma \
			--without-tiff \
			--without-trio \
			--without-wmf \
			--without-xml \
			--without-x)
	$(TOUCH) $(STAGEFILES_DIR)/.graphicsmagick_configured

#
# compile graphicsmagick
#

$(STAGEFILES_DIR)/.graphicsmagick_compiled: $(STAGEFILES_DIR)/.graphicsmagick_configured
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(GRAPHICSMAGICK_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.graphicsmagick_compiled

#
# install graphicsmagick
#

$(STAGEFILES_DIR)/.graphicsmagick_installed: $(STAGEFILES_DIR)/.graphicsmagick_compiled
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(GRAPHICSMAGICK_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.graphicsmagick_installed


$(FILELIST_DIR)/graphicsmagick.lst: $(STAGEFILES_DIR)/.graphicsmagick_installed
	$(TOUCH) $(FILELIST_DIR)/graphicsmagick.lst

.PHONY: clean-graphicsmagick distclean-graphicsmagick

clean-graphicsmagick:
	-$(RM) -rf $(GRAPHICSMAGICK_DIR)

#
# clean everthing else
#

distclean-graphicsmagick:
	-$(RM) -f $(STAGEFILES_DIR)/.graphicsmagick_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.graphicsmagick_patched
	-$(RM) -f $(STAGEFILES_DIR)/.graphicsmagick_configured
	-$(RM) -f $(STAGEFILES_DIR)/.graphicsmagick_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.graphicsmagick_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GRAPHICSMAGICK_DLFILE)
endif
