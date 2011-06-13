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
# The project's page is at http://www.open7x0.org
#

ifeq ($(CONFIG_MEDIATOMB),y)
ifneq ($(and $(filter y,$(CONFIG_SQLITE)),$(filter y,$(CONFIG_EXPAT))),y)
   $(error dependency error: mediatomb needs sqlite and expat enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
MEDIATOMB_DEPS = $(BASE_BUILD_STAGEFILE) $(SQLITE_INSTALLED) $(EXPAT_INSTALLED)

ifeq ($(CONFIG_ZLIB),y)
	MEDIATOMB_DEPS +=  $(ZLIB_INSTALLED)
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

MEDIATOMB_VERSION := 0.12.1
MEDIATOMB_PATCHES_DIR := $(PATCHES_DIR)/mediatomb/$(MEDIATOMB_VERSION)

MEDIATOMB_FILE := mediatomb-$(MEDIATOMB_VERSION).tar.gz
MEDIATOMB_DLFILE := $(DOWNLOAD_DIR)/$(MEDIATOMB_FILE)
MEDIATOMB_URL := http://downloads.sourceforge.net/mediatomb/$(MEDIATOMB_FILE)
MEDIATOMB_DIR := $(BUILD_DIR)/mediatomb-$(MEDIATOMB_VERSION)

MEDIATOMB_INSTALLED = $(STAGEFILES_DIR)/.mediatomb_installed

PACKS_RULES_$(CONFIG_MEDIATOMB) += $(MEDIATOMB_INSTALLED)
FILE_LISTS_$(CONFIG_MEDIATOMB) += mediatomb.lst

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
	$(TAR) -C $(BUILD_DIR) -zf $(MEDIATOMB_DLFILE)
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
	($(CD) $(MEDIATOMB_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="-L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib" \
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
			--disable-log \
			--disable-debug-log \
			--disable-external-transcoding \
			$(if $(CONFIG_ZLIB),--enable-zlib,--disable-zlib) \
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
			--disable-libextractor \
			--disable-lastfmlib \
			--disable-libmp4v2)
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_configured

#
# compile mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_compiled: $(STAGEFILES_DIR)/.mediatomb_configured
	$(UCLIBC_ENV) $(MAKE) -C $(MEDIATOMB_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_compiled

#
# install mediatomb
#

$(STAGEFILES_DIR)/.mediatomb_installed: $(STAGEFILES_DIR)/.mediatomb_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(MEDIATOMB_DIR) install
	-$(RM) -rf $(TARGET_ROOT)/etc/mediatomb
	$(MV) $(TARGET_ROOT)/usr/share/mediatomb $(TARGET_ROOT)/etc
	$(CAT) $(TARGET_ROOT)/etc/mediatomb/sqlite3.sql | \
		$(HOSTUTILS_PREFIX_BIN)/sqlite3 \
		$(TARGET_ROOT)/etc/mediatomb/mediatomb.db
	$(TOUCH) $(STAGEFILES_DIR)/.mediatomb_installed


$(FILELIST_DIR)/mediatomb.lst: $(STAGEFILES_DIR)/.mediatomb_installed
	$(TOUCH) $(FILELIST_DIR)/mediatomb.lst

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
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MEDIATOMB_DLFILE)
endif