# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2021 VDR-NG-EM Project
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

ifeq ($(CONFIG_FUPPES),y)
ifneq ($(and $(filter y,$(CONFIG_PCRE)),$(filter y,$(CONFIG_LIBXML2)),$(filter y,$(CONFIG_SQLITE))),y)
   $(error dependency error: fuppes needs pcre, libxml2 and sqlite enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
FUPPES_DEPS = $(BASE_BUILD_STAGEFILE) $(PCRE_INSTALLED) $(LIBXML2_INSTALLED) $(SQLITE_INSTALLED)

ifeq ($(CONFIG_TAGLIB),y)
	FUPPES_DEPS +=  $(TAGLIB_INSTALLED)
endif
ifeq ($(CONFIG_LIBUUID),y)
	FUPPES_DEPS +=  $(LIBUUID_INSTALLED)
endif
ifeq ($(CONFIG_LIBICONV),y)
	FUPPES_DEPS +=  $(LIBICONV_INSTALLED)
endif
ifeq ($(CONFIG_FFMPEG),y)
	FUPPES_DEPS +=  $(FFMPEG_INSTALLED)
	ifeq ($(CONFIG_LIBFFMPEGTHUMBNAILER),y)
		FUPPES_DEPS +=  $(LIBFFMPEGTHUMBNAILER_INSTALLED)
	endif
endif

FUPPES_VERSION := 0.660
FUPPES_PATCHES_DIR := $(PATCHES_DIR)/fuppes/$(FUPPES_VERSION)

FUPPES_FILE := fuppes-$(FUPPES_VERSION).tar.gz
FUPPES_DLFILE := $(DOWNLOAD_DIR)/$(FUPPES_FILE)
FUPPES_URL := http://downloads.sourceforge.net/fuppes/$(FUPPES_FILE)
FUPPES_DIR := $(BUILD_DIR)/fuppes-$(FUPPES_VERSION)
FUPPES_LIBNAMES := $(TARGET_ROOT)/usr/lib/fuppes

FUPPES_INSTALLED = $(STAGEFILES_DIR)/.fuppes_installed

PACKS_RULES_$(CONFIG_FUPPES) += $(FUPPES_INSTALLED)
FILE_LISTS_$(CONFIG_FUPPES) += fuppes.lst

CLEAN_RULES += clean-fuppes
DISTCLEAN_RULES += distclean-fuppes

#
# download fuppes
#

$(FUPPES_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(FUPPES_DLFILE) ] ; then \
	$(WGET) $(FUPPES_URL) -O $(FUPPES_DLFILE) ; \
	fi );
	$(TOUCH) $(FUPPES_DLFILE)

#
# unpack fuppes
#

$(STAGEFILES_DIR)/.fuppes_unpacked: $(FUPPES_DLFILE) \
                                           $(wildcard $(FUPPES_PATCHES_DIR)/*.patch) \
                                           $$(FUPPES_DEPS)
	-$(RM) -rf $(FUPPES_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(FUPPES_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.fuppes_unpacked

#
# patch fuppes
#

$(STAGEFILES_DIR)/.fuppes_patched: $(STAGEFILES_DIR)/.fuppes_unpacked
	$(call patch_package, $(FUPPES_DIR), $(FUPPES_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.fuppes_patched

#
# configure fuppes
# 

$(STAGEFILES_DIR)/.fuppes_configured: $(STAGEFILES_DIR)/.fuppes_patched
	($(CD) $(FUPPES_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="$(UCLIBC_LDFLAGS) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib -lpthread -lrt -ldl" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(FUPPES_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-rpath \
			$(if $(CONFIG_TAGLIB),--enable-taglib,--disable-taglib) \
			$(if $(CONFIG_FFMPEG),--enable-libavformat,--disable-libavformat) \
			$(if $(filter yy,$(CONFIG_FFMPEG)$(CONFIG_LIBFFMPEGTHUMBNAILER)),--enable-ffmpegthumbnailer,--disable-ffmpegthumbnailer) \
			$(if $(CONFIG_LIBICONV),--with-libiconv-prefix=$(TARGET_ROOT)/usr/lib/libiconv,--without-libiconv-prefix) \
			--disable-magickwand \
			--disable-windows-service \
			--disable-exiv2 \
			--disable-transcoder-ffmpeg \
			--disable-mp4v2 \
			--disable-lame \
			--disable-twolame \
			--disable-vorbis \
			--disable-tremor \
			--disable-musepack \
			--disable-flac \
			--disable-mad \
			--disable-faad \
			--enable-dlna \
			--disable-inotify)
	$(TOUCH) $(STAGEFILES_DIR)/.fuppes_configured

#
# compile fuppes
#

$(STAGEFILES_DIR)/.fuppes_compiled: $(STAGEFILES_DIR)/.fuppes_configured
	$(UCLIBC_ENV) $(MAKE) -C $(FUPPES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.fuppes_compiled

#
# install fuppes
#

$(STAGEFILES_DIR)/.fuppes_installed: $(STAGEFILES_DIR)/.fuppes_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(FUPPES_DIR) install
	-$(RM) -f $(FUPPES_LIBNAMES)/libencoder*
	$(TOUCH) $(STAGEFILES_DIR)/.fuppes_installed


$(FILELIST_DIR)/fuppes.lst: $(STAGEFILES_DIR)/.fuppes_installed
	($(CD) $(FUPPES_LIBNAMES); \
	$(ECHO) "usr/bin/fuppes              usr/bin/fuppes              f 755 0 0 - - - - -"; \
	$(ECHO) "usr/bin/fuppes              -                           s   - - - - - - - -"; \
	$(ECHO) "usr/bin/fuppesd             usr/bin/fuppesd             f 755 0 0 - - - - -"; \
	$(ECHO) "usr/bin/fuppesd             -                           s   - - - - - - - -"; \
	$(ECHO) "usr/lib/libfuppes.so.0.0.0  usr/lib/libfuppes.so.0.0.0  f 755 0 0 - - - - -"; \
	$(ECHO) "usr/lib/libfuppes.so.0.0.0  -                           s   - - - - - - - -"; \
	$(ECHO) "usr/lib/libfuppes.so.0      libfuppes.so.0.0.0          l 777 0 0 - - - - -"; \
	$(ECHO) "usr/lib/libfuppes.so        libfuppes.so.0.0.0          l 777 0 0 - - - - -"; \
	$(ECHO) "usr/lib/fuppes              -                           d 755 0 0 - - - - -"; \
	for lib in lib*.so.0.0.0 ; do \
		$(ECHO) "usr/lib/fuppes/$$lib  usr/lib/fuppes/$$lib  f 755 0 0 - - - - -"; \
		$(ECHO) "usr/lib/fuppes/$$lib  -                     s   - - - - - - - -"; \
		$(FIND) . -lname "*$$lib" \
			-printf "usr/lib/fuppes/%-24P %-18l l 777 0 0 - - - - -\n"; \
	done) > $(FILELIST_DIR)/fuppes.lst

.PHONY: clean-fuppes distclean-fuppes

clean-fuppes:
	-$(RM) -rf $(FUPPES_DIR)

#
# clean everthing else
#

distclean-fuppes:
	-$(RM) -f $(STAGEFILES_DIR)/.fuppes_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.fuppes_patched
	-$(RM) -f $(STAGEFILES_DIR)/.fuppes_configured
	-$(RM) -f $(STAGEFILES_DIR)/.fuppes_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.fuppes_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(FUPPES_DLFILE)
endif
