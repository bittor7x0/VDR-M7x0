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
#
# The project's page is at http://www.open7x0.org
#
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
FFMPEG_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_ZLIB),y)
   FFMPEG_DEPS += $(ZLIB_INSTALLED)
endif

FFMPEG_VERSION := 0.5.2
FFMPEG_PATCHES_DIR := $(PATCHES_DIR)/ffmpeg/$(FFMPEG_VERSION)

FFMPEG_FILE := ffmpeg-$(FFMPEG_VERSION).tar.bz2
FFMPEG_DLFILE := $(DOWNLOAD_DIR)/$(FFMPEG_FILE)
FFMPEG_URL := http://ffmpeg.org/releases/$(FFMPEG_FILE)
FFMPEG_DIR := $(BUILD_DIR)/ffmpeg-$(FFMPEG_VERSION)

FFMPEG_INSTALLED = $(STAGEFILES_DIR)/.ffmpeg_installed

PACKS_RULES_$(CONFIG_FFMPEG) += $(FFMPEG_INSTALLED)
FILE_LISTS_$(CONFIG_FFMPEG) += ffmpeg.lst

CLEAN_RULES += clean-ffmpeg
DISTCLEAN_RULES += distclean-ffmpeg

ifneq ($(CONFIG_LIBDLNA),y)
	FFMPEG_CONFIGURE_OPTIONS:=--enable-cross-compile \
			--cross-prefix=$(UCLIBC_TARGET)- \
			--arch=mips \
			--prefix=/usr \
			--enable-shared \
			--enable-static \
			--disable-debug \
			--enable-ffmpeg \
			--disable-ffplay \
			--disable-ffserver \
			--disable-network \
			--enable-gpl \
			--disable-libfaad \
			--disable-mmx \
			--disable-mmx2 \
			--enable-pthreads \
			$(if $(CONFIG_SQUASHFS_LZMA),,--disable-optimizations --enable-small) \
			--disable-stripping \
			--disable-vhook \
			$(if $(CONFIG_ZLIB),--enable-zlib,--disable-zlib) \
			--enable-postproc \
			--disable-ipv6 \
			--disable-bsfs \
			--disable-devices \
			--disable-encoders \
			--enable-encoder=ac3 \
			--enable-encoder=mpeg1video \
			--enable-encoder=mpeg2video \
			--enable-encoder=pcm_s16be \
			--enable-encoder=pcm_s16le \
			$(if $(CONFIG_ZLIB),--enable-encoder=zlib,--disable-encoder=zlib) \
			--disable-decoders \
			--enable-decoder=ac3 \
			--enable-decoder=mpeg1video \
			--enable-decoder=mpeg2video \
			--enable-decoder=mpegvideo \
			--enable-decoder=pcm_s16be \
			--enable-decoder=pcm_s16le \
			$(if $(CONFIG_ZLIB),--enable-decoder=zlib,--disable-decoder=zlib) \
			--disable-muxers \
			--enable-muxer=ac3 \
			--enable-muxer=mpeg1video \
			--enable-muxer=mpeg2video \
			--enable-muxer=mpegts \
			--disable-demuxers \
			--enable-demuxer=ac3 \
			--enable-demuxer=mpegps \
			--enable-demuxer=mpegts \
			--enable-demuxer=mpegvideo \
			--enable-demuxer=rm \
			--enable-demuxer=v4l2 \
			--disable-parsers \
			--enable-parser=ac3 \
			--enable-parser=mpegaudio \
			--enable-parser=mpegvideo \
			--disable-protocols \
			--enable-protocol=file \
			--enable-protocol=pipe \
			--disable-filters
else
	FFMPEG_CONFIGURE_OPTIONS:=--enable-cross-compile \
			--cross-prefix=$(UCLIBC_TARGET)- \
			--arch=mips \
			--prefix=/usr \
			--enable-shared \
			--enable-static \
			--disable-debug \
			--disable-ffmpeg \
			--disable-ffplay \
			--disable-ffserver \
			--disable-network \
			--enable-gpl \
			--disable-libfaad \
			--disable-mmx \
			--disable-mmx2 \
			--enable-pthreads \
			$(if $(CONFIG_SQUASHFS_LZMA),,--disable-optimizations --enable-small) \
			--disable-stripping \
			--disable-vhook \
			$(if $(CONFIG_ZLIB),--enable-zlib,--disable-zlib) \
			--enable-postproc \
			--disable-ipv6 \
			--disable-bsfs \
			--disable-devices \
			--disable-encoders \
			--disable-decoders \
			--enable-decoder=ac3 \
			--enable-decoder=atrac3 \
			--enable-decoder=h264 \
			--enable-decoder=jpegls \
			--enable-decoder=mp3 \
			--enable-decoder=mpeg1video \
			--enable-decoder=mpeg2video \
			--enable-decoder=mpeg4 \
			--enable-decoder=mpeg4aac \
			--enable-decoder=mpegvideo \
			--enable-decoder=wmav1 \
			--enable-decoder=wmav2 \
			--enable-decoder=png \
			$(if $(CONFIG_ZLIB),--enable-decoder=zlib,--disable-decoder=zlib) \
			--disable-muxers \
			--disable-demuxers \
			--enable-demuxer=ac3 \
			--enable-demuxer=h264 \
			--enable-demuxer=mp3 \
			--enable-demuxer=mpegvideo \
			--disable-parsers \
			--enable-parser=aac \
			--enable-parser=ac3 \
			--enable-parser=h264 \
			--enable-parser=mpegaudio \
			--enable-parser=mpegvideo \
			--enable-parser=mpeg4video \
			--disable-protocols \
			--disable-filters
endif

#
# download ffmpeg
#

$(FFMPEG_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(FFMPEG_DLFILE) ] ; then \
	$(WGET) $(FFMPEG_URL) -O $(FFMPEG_DLFILE) ; \
	fi );
	$(TOUCH) $(FFMPEG_DLFILE)

#
# unpack ffmpeg
#

$(STAGEFILES_DIR)/.ffmpeg_unpacked: $(FFMPEG_DLFILE) \
                                           $(wildcard $(FFMPEG_PATCHES_DIR)/*.patch) \
                                           $$(FFMPEG_DEPS)
	-$(RM) -rf $(FFMPEG_DIR)
	$(BZCAT) $(FFMPEG_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.ffmpeg_unpacked

#
# patch ffmpeg
#

$(STAGEFILES_DIR)/.ffmpeg_patched: $(STAGEFILES_DIR)/.ffmpeg_unpacked
	$(call patch_package, $(FFMPEG_DIR), $(FFMPEG_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ffmpeg_patched

#
# configure ffmpeg
#

$(STAGEFILES_DIR)/.ffmpeg_configured: $(STAGEFILES_DIR)/.ffmpeg_patched
	($(CD) $(FFMPEG_DIR) ; $(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" \
		$(FFMPEG_DIR)/configure $(FFMPEG_CONFIGURE_OPTIONS))
	$(TOUCH) $(STAGEFILES_DIR)/.ffmpeg_configured

#
# compile ffmpeg
#

$(STAGEFILES_DIR)/.ffmpeg_compiled: $(STAGEFILES_DIR)/.ffmpeg_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(FFMPEG_DIR) \
		DESTDIR="$(TARGET_ROOT)" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.ffmpeg_compiled

#
# install ffmpeg
#

$(STAGEFILES_DIR)/.ffmpeg_installed: $(STAGEFILES_DIR)/.ffmpeg_compiled
	$(UCLIBC_ENV) $(MAKE) \
		-C $(FFMPEG_DIR) \
		DESTDIR="$(TARGET_ROOT)" \
		all install
	$(TOUCH) $(STAGEFILES_DIR)/.ffmpeg_installed


$(FILELIST_DIR)/ffmpeg.lst: $(STAGEFILES_DIR)/.ffmpeg_installed
ifneq ($(CONFIG_LIBDLNA),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_bin.lst $(FILELIST_DIR)/ffmpeg_libs.lst > $(FILELIST_DIR)/ffmpeg.lst
else
	$(CP) -f $(FILELIST_DIR)/ffmpeg_libs.lst $(FILELIST_DIR)/ffmpeg.lst
endif
	$(TOUCH) -f  $(FILELIST_DIR)/ffmpeg.lst

.PHONY: clean-ffmpeg distclean-ffmpeg

clean-ffmpeg:
	-$(RM) -rf $(FFMPEG_DIR)

#
# clean everthing else
#

distclean-ffmpeg:
	-$(RM) -f $(STAGEFILES_DIR)/.ffmpeg_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ffmpeg_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ffmpeg_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ffmpeg_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ffmpeg_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(FFMPEG_DLFILE)
endif
