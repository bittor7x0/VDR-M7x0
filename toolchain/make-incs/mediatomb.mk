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

ifeq ($(CONFIG_MEDIATOMB),y)
ifneq ($(and $(filter y,$(CONFIG_SQLITE)),$(filter y,$(CONFIG_EXPAT))),y)
   $(error dependency error: mediatomb needs sqlite and expat enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
MEDIATOMB_DEPS = $(BASE_BUILD_STAGEFILE) $(SQLITE_INSTALLED) $(EXPAT_INSTALLED)

ifeq ($(CONFIG_UCLIBC++),y)
	MEDIATOMB_DEPS +=  $(UCLIBC++_INSTALLED)
endif
ifeq ($(CONFIG_TAGLIB),y)
	MEDIATOMB_DEPS +=  $(TAGLIB_INSTALLED)
endif
ifeq ($(CONFIG_ID3LIB),y)
	MEDIATOMB_DEPS +=  $(ID3LIB_INSTALLED)
endif
ifeq ($(CONFIG_FFMPEG),y)
	MEDIATOMB_DEPS +=  $(FFMPEG_INSTALLED)
	ifeq ($(CONFIG_LIBFFMPEGTHUMBNAILER),y)
		MEDIATOMB_DEPS +=  $(LIBFFMPEGTHUMBNAILER_INSTALLED)
	endif
endif
ifeq ($(CONFIG_CURL),y)
	MEDIATOMB_DEPS +=  $(CURL_INSTALLED)
endif
ifeq ($(CONFIG_LIBEXIF),y)
	MEDIATOMB_DEPS +=  $(LIBEXIF_INSTALLED)
endif
ifeq ($(CONFIG_LIBJS),y)
	MEDIATOMB_DEPS +=  $(LIBJS_INSTALLED)
endif
ifeq ($(CONFIG_LIBMAGIC),y)
	MEDIATOMB_DEPS +=  $(LIBMAGIC_INSTALLED)
endif
ifeq ($(CONFIG_YUI_COMPRESSOR),y)
	MEDIATOMB_DEPS +=  $(YUI_COMPRESSOR_JAR)
endif
ifeq ($(CONFIG_PNGOUT),y)
	MEDIATOMB_DEPS +=  $(PNGOUT_BIN)
endif
ifeq ($(CONFIG_ZOPFLIPNG),y)
	MEDIATOMB_DEPS +=  $(ZOPFLIPNG_BIN)
endif
ifeq ($(CONFIG_PNGWOLF-ZOPFLI),y)
	MEDIATOMB_DEPS +=  $(PNGWOLF-ZOPFLI_BIN)
endif
ifeq ($(CONFIG_ECT),y)
	MEDIATOMB_DEPS +=  $(ECT_BIN)
endif
ifeq ($(CONFIG_PINGO),y)
	MEDIATOMB_DEPS +=  $(PINGO_BIN)
endif

MEDIATOMB_VERSION := 0.12.1-47-g7ab7616
MEDIATOMB_PATCHES_DIR := $(PATCHES_DIR)/mediatomb/$(MEDIATOMB_VERSION)

MEDIATOMB_FILE := mediatomb_$(MEDIATOMB_VERSION).orig.tar.xz
MEDIATOMB_DLFILE := $(DOWNLOAD_DIR)/$(MEDIATOMB_FILE)
MEDIATOMB_URL := http://old-releases.ubuntu.com/ubuntu/pool/universe/m/mediatomb/$(MEDIATOMB_FILE)
MEDIATOMB_DIR := $(BUILD_DIR)/mediatomb-$(MEDIATOMB_VERSION)
MEDIATOMB_CONF_DIR := $(PRG_CONFIGS_DIR)/mediatomb

MEDIATOMB_INSTALLED = $(STAGEFILES_DIR)/.mediatomb_installed

PACKS_RULES_$(CONFIG_MEDIATOMB) += $(MEDIATOMB_INSTALLED)
FILE_LISTS_$(CONFIG_MEDIATOMB) += mediatomb.lst mediatomb-configs.lst

CLEAN_RULES += clean-mediatomb
DISTCLEAN_RULES += distclean-mediatomb

#
# download mediatomb
#

$(MEDIATOMB_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MEDIATOMB_DLFILE) ] ; then \
	$(WGET) $(MEDIATOMB_URL) -O $(MEDIATOMB_DLFILE) ; \
	fi );
	$(TOUCH) $(MEDIATOMB_DLFILE)

#
# unpack mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_unpacked: $(MEDIATOMB_DLFILE) \
                                           $(wildcard $(MEDIATOMB_PATCHES_DIR)/*.patch) \
                                           $$(MEDIATOMB_DEPS)
	-$(RM) -rf $(MEDIATOMB_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(MEDIATOMB_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_unpacked

#
# patch mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_patched: $(STAGEFILES_DIR)/.mediatomb_unpacked
	$(call patch_package, $(MEDIATOMB_DIR), $(MEDIATOMB_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_patched

#
# configure mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_configured: $(STAGEFILES_DIR)/.mediatomb_patched
	($(CD) $(MEDIATOMB_DIR) ; autoreconf -i ; $(UCLIBC_ENV_LTO_GC) \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		LDFLAGS="-flto -fwhole-program -Wl,--gc-sections -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(MEDIATOMB_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-sqlite3 \
			--disable-mysql \
			--disable-db-autocreate \
			--enable-pthread-lib \
			--disable-rpl-malloc \
			--disable-tombdebug \
			--disable-upnpdebug \
			--enable-log \
			--disable-debug-log \
			--disable-external-transcoding \
			$(if $(filter yy,$(CONFIG_TAGLIB)$(CONFIG_ID3LIB)),--enable-taglib --with-taglib-cfg=$(TARGET_ROOT)/usr/bin/taglib-config --disable-id3lib, \
			$(if $(CONFIG_TAGLIB),--enable-taglib --with-taglib-cfg=$(TARGET_ROOT)/usr/bin/taglib-config,--disable-taglib) \
			$(if $(CONFIG_ID3LIB),--enable-id3lib,--disable-id3lib)) \
			$(if $(CONFIG_FFMPEG),--enable-ffmpeg,--disable-ffmpeg) \
			$(if $(filter yy,$(CONFIG_FFMPEG)$(CONFIG_LIBFFMPEGTHUMBNAILER)),--enable-ffmpegthumbnailer,--disable-ffmpegthumbnailer) \
			$(if $(CONFIG_CURL),--enable-curl --with-curl-cfg=$(TARGET_ROOT)/usr/bin/curl-config --enable-youtube,--disable-curl --disable-youtube) \
			$(if $(CONFIG_LIBEXIF),--enable-libexif,--disable-libexif) \
			$(if $(CONFIG_LIBMAGIC),--enable-libmagic,--disable-libmagic) \
			$(if $(CONFIG_LIBJS),--enable-libjs --with-js-h=$(TARGET_ROOT)/usr/include/js,--disable-libjs) \
			--disable-inotify \
			--disable-lastfm \
			--disable-flac \
			--disable-libmp4v2)
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_configured

#
# compile mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_compiled: $(STAGEFILES_DIR)/.mediatomb_configured
	$(UCLIBC_ENV_LTO_GC) \
	$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
	$(MAKE) -C $(MEDIATOMB_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_compiled

#
# install mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_installed: $(STAGEFILES_DIR)/.mediatomb_compiled
	$(UCLIBC_ENV_LTO_GC) \
	$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
	$(MAKE) -C $(MEDIATOMB_DIR) install
	-$(RM) -rf $(TARGET_ROOT)/etc/mediatomb
	$(MV) $(TARGET_ROOT)/usr/share/mediatomb $(TARGET_ROOT)/etc
	$(CAT) $(TARGET_ROOT)/etc/mediatomb/sqlite3.sql | \
		$(HOSTUTILS_PREFIX_BIN)/sqlite3 \
		$(TARGET_ROOT)/etc/mediatomb/mediatomb.db
ifeq ($(CONFIG_YUI_COMPRESSOR),y)
	$(call css_shrink_dir, $(TARGET_ROOT)/etc/mediatomb/web)
	$(call js_shrink_dir, $(TARGET_ROOT)/etc/mediatomb/js)
	$(call js_shrink_dir, $(TARGET_ROOT)/etc/mediatomb/web/js)
endif
ifeq ($(or $(CONFIG_PNGOUT),$(CONFIG_ZOPFLIPNG),$(CONFIG_PNGWOLF-ZOPFLI),$(CONFIG_ECT),$(CONFIG_PINGO)),y)
	$(call png_shrink_dir, $(TARGET_ROOT)/etc/mediatomb/web/icons)
endif
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_installed


$(FILELIST_DIR)/mediatomb.lst: $(STAGEFILES_DIR)/.mediatomb_installed
	$(TOUCH) $(FILELIST_DIR)/mediatomb.lst

$(eval $(call gen_copy_file_lst,$(MEDIATOMB_CONF_DIR),,0,0,mediatomb-configs.lst,check))

.PHONY: clean-mediatomb distclean-mediatomb

clean-mediatomb:
	-$(RM) -rf $(MEDIATOMB_DIR)

#
# clean everthing else
#

distclean-mediatomb:
	-$(RM) -f $(STAGEFILES_DIR)/.mediatomb_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mediatomb_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mediatomb_configured
	-$(RM) -f $(STAGEFILES_DIR)/.mediatomb_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mediatomb_installed
	-$(RM) -f $(FILELIST_DIR)/mediatomb-configs.lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MEDIATOMB_DLFILE)
endif
