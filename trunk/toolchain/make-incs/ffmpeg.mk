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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
FFMPEG_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_ZLIB),y)
   FFMPEG_DEPS += $(ZLIB_INSTALLED)
endif

FFMPEG_VERSION := 1.0
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
		LDFLAGS="$(UCLIBC_LDFLAGS) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(FFMPEG_DIR)/configure \
			--enable-cross-compile \
			--cross-prefix=$(UCLIBC_TARGET)- \
			--arch=mips \
			--target-os=linux \
			--prefix="/usr" \
			--enable-shared \
			--enable-static \
			--pkg-config=pkg-config \
			--disable-debug \
			--disable-ffmpeg \
			--disable-ffplay \
			--disable-ffprobe \
			--disable-ffserver \
			--disable-network \
			--enable-gpl \
			--enable-version3 \
			--disable-doc \
			--disable-htmlpages \
			--disable-manpages \
			--disable-podpages \
			--disable-txtpages \
			--disable-dxva2 \
			--disable-vaapi \
			--disable-vda \
			--disable-vdpau \
			--disable-mmx \
			--disable-mmxext \
			--disable-sse \
			--disable-sse2 \
			--disable-sse3 \
			--disable-ssse3 \
			--disable-sse4 \
			--disable-sse42 \
			--disable-mips32r2 \
			--disable-mipsdspr1 \
			--disable-mipsdspr2 \
			--disable-mipsfpu \
			--disable-fast-unaligned \
			--enable-pthreads \
			--disable-stripping \
			$(if $(CONFIG_FFMPEG_SIZE),--disable-optimizations --enable-small,) \
			$(if $(CONFIG_ZLIB),--enable-zlib,--disable-zlib) \
			$(if $(CONFIG_FFMPEG_LIBAVCODEC),--enable-avcodec,--disable-avcodec) \
			$(if $(CONFIG_FFMPEG_LIBAVDEVICE),--enable-avdevice,--disable-avdevice) \
			$(if $(CONFIG_FFMPEG_LIBAVFORMAT),--enable-avformat,--disable-avformat) \
			$(if $(CONFIG_FFMPEG_LIBPOSTPROC),--enable-postproc,--disable-postproc) \
			$(if $(CONFIG_FFMPEG_LIBSWSCALE),--enable-swscale,--disable-swscale) \
			--disable-bsfs \
			--disable-devices \
			--disable-encoders \
			--disable-decoders \
			--enable-decoder=ac3 \
			--enable-decoder=h264 \
			--enable-decoder=mpeg1video \
			--enable-decoder=mpeg_xvmc \
			--enable-decoder=mpeg2video \
			--enable-decoder=h264 \
			--enable-decoder=mpeg4 \
			--enable-decoder=mpeg4aac \
			--enable-decoder=mpegvideo \
			--enable-decoder=pcm_s16be \
			--enable-decoder=pcm_s16le \
			$(if $(CONFIG_ZLIB),--enable-decoder=zlib,--disable-decoder=zlib) \
			--disable-muxers \
			--disable-demuxers \
			--enable-demuxer=ac3 \
			--enable-demuxer=h264 \
			--enable-demuxer=mpegps \
			--enable-demuxer=mpegts \
			--enable-demuxer=mpegvideo \
			--disable-parsers \
			--enable-parser=aac \
			--enable-parser=ac3 \
			--enable-parser=h264 \
			--enable-parser=mpegaudio \
			--enable-parser=mpegvideo \
			--enable-parser=mpeg4video \
			--disable-protocols \
			--disable-avfilter \
			--disable-filters \
			--disable-outdevs)
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
	-$(RM) -f $(FILELIST_DIR)/ffmpeg.lst
	$(TOUCH) -f $(FILELIST_DIR)/ffmpeg.lst
ifeq ($(CONFIG_FFMPEG_LIBAVCODEC),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libavcodec.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
ifeq ($(CONFIG_FFMPEG_LIBAVDEVICE),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libavdevice.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
ifeq ($(CONFIG_FFMPEG_LIBAVFORMAT),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libavformat.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
ifeq ($(CONFIG_FFMPEG_LIBAVUTIL),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libavutil.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
ifeq ($(CONFIG_FFMPEG_LIBPOSTPROC),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libpostproc.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
ifeq ($(CONFIG_FFMPEG_LIBSWSCALE),y)
	$(CAT) $(FILELIST_DIR)/ffmpeg_libswscale.lst >> $(FILELIST_DIR)/ffmpeg.lst
endif
	$(TOUCH) -f $(FILELIST_DIR)/ffmpeg.lst

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
	-$(RM) -f $(FILELIST_DIR)/ffmpeg.lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(FFMPEG_DLFILE)
endif
