# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 VDR-NG-EM Project
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

ifeq ($(CONFIG_NOAD),y)
ifneq ($(or $(CONFIG_FFMPEG),$(CONFIG_LIBMPEG2)),y)
   $(error dependency error: noad needs ffmpeg or libmpeg2 enabled)
endif
ifeq ($(CONFIG_FFMPEG),y)
ifneq ($(and $(filter y,$(CONFIG_FFMPEG_LIBAVCODEC)),$(filter y,$(CONFIG_FFMPEG_LIBAVUTIL)),$(filter y,$(CONFIG_FFMPEG_LIBAVFORMAT))),y)
   $(error dependency error: noad needs ffmpeg with libavcodec, libavutil and libavformat libraries enabled)
endif
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
NOAD_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_FFMPEG),y)
	NOAD_DEPS +=  $(FFMPEG_INSTALLED)
endif
ifeq ($(CONFIG_LIBMPEG2),y)
	NOAD_DEPS +=  $(LIBMPEG2_INSTALLED)
endif

NOAD_VERSION := 0.8.5
NOAD_PATCHES_DIR := $(PATCHES_DIR)/noad/$(NOAD_VERSION)

NOAD_FILE := noad-$(NOAD_VERSION).tar.bz2
NOAD_DLFILE := $(DOWNLOAD_DIR)/$(NOAD_FILE)
NOAD_URL := http://noad.net23.net/$(NOAD_FILE)
NOAD_DIR := $(BUILD_DIR)/noad-$(NOAD_VERSION)

NOAD_INSTALLED = $(STAGEFILES_DIR)/.noad_installed

PACKS_RULES_$(CONFIG_NOAD) += $(NOAD_INSTALLED)
FILE_LISTS_$(CONFIG_NOAD) += noad.lst

CLEAN_RULES += clean-noad
DISTCLEAN_RULES += distclean-noad

#
# download noad
#

$(NOAD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NOAD_DLFILE) ] ; then \
	$(WGET) $(NOAD_URL) -O $(NOAD_DLFILE) ; \
	fi );
	$(TOUCH) $(NOAD_DLFILE)

#
# unpack noad
#

$(STAGEFILES_DIR)/.noad_unpacked: $(NOAD_DLFILE) \
                                           $(wildcard $(NOAD_PATCHES_DIR)/*.patch) \
                                           $$(NOAD_DEPS)
	-$(RM) -rf $(NOAD_DIR)
	$(BZCAT) $(NOAD_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.noad_unpacked

#
# patch noad
#

$(STAGEFILES_DIR)/.noad_patched: $(STAGEFILES_DIR)/.noad_unpacked
	$(call patch_package, $(NOAD_DIR), $(NOAD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.noad_patched

#
# configure noad
#

$(STAGEFILES_DIR)/.noad_configured: $(STAGEFILES_DIR)/.noad_patched
	($(CD) $(NOAD_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="$(UCLIBC_LDFLAGS) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(NOAD_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			$(if $(CONFIG_FFMPEG),--with-ffmpeg,--without-ffmpeg) \
			$(if $(CONFIG_LIBMPEG2),--with-libmpeg2,--without-libmpeg2) \
			--without-magick)
	$(TOUCH) $(STAGEFILES_DIR)/.noad_configured

#
# compile noad
#

$(STAGEFILES_DIR)/.noad_compiled: $(STAGEFILES_DIR)/.noad_configured
	$(UCLIBC_ENV) $(MAKE) -C $(NOAD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.noad_compiled

#
# install noad
#

$(STAGEFILES_DIR)/.noad_installed: $(STAGEFILES_DIR)/.noad_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(NOAD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.noad_installed


$(FILELIST_DIR)/noad.lst: $(STAGEFILES_DIR)/.noad_installed
	$(TOUCH) $(FILELIST_DIR)/noad.lst

.PHONY: clean-noad distclean-noad

clean-noad:
	-$(RM) -rf $(NOAD_DIR)

#
# clean everthing else
#

distclean-noad:
	-$(RM) -f $(STAGEFILES_DIR)/.noad_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.noad_patched
	-$(RM) -f $(STAGEFILES_DIR)/.noad_configured
	-$(RM) -f $(STAGEFILES_DIR)/.noad_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.noad_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NOAD_DLFILE)
endif
