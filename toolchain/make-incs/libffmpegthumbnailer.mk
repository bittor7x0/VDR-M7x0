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

ifeq ($(CONFIG_LIBFFMPEGTHUMBNAILER),y)
ifneq ($(CONFIG_FFMPEG),y)
   $(error dependency error: libffmpegthumbnailer needs ffmpeg enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBFFMPEGTHUMBNAILER_DEPS = $(BASE_BUILD_STAGEFILE) $(FFMPEG_INSTALLED)

LIBFFMPEGTHUMBNAILER_VERSION := 2.0.4
LIBFFMPEGTHUMBNAILER_PATCHES_DIR := $(PATCHES_DIR)/libffmpegthumbnailer/$(LIBFFMPEGTHUMBNAILER_VERSION)

LIBFFMPEGTHUMBNAILER_FILE := ffmpegthumbnailer-$(LIBFFMPEGTHUMBNAILER_VERSION).tar.gz
LIBFFMPEGTHUMBNAILER_DLFILE := $(DOWNLOAD_DIR)/$(LIBFFMPEGTHUMBNAILER_FILE)
LIBFFMPEGTHUMBNAILER_URL := http://ffmpegthumbnailer.googlecode.com/files/$(LIBFFMPEGTHUMBNAILER_FILE)
LIBFFMPEGTHUMBNAILER_DIR := $(BUILD_DIR)/ffmpegthumbnailer-$(LIBFFMPEGTHUMBNAILER_VERSION)

LIBFFMPEGTHUMBNAILER_INSTALLED = $(STAGEFILES_DIR)/.libffmpegthumbnailer_installed

PACKS_RULES_$(CONFIG_LIBFFMPEGTHUMBNAILER) += $(LIBFFMPEGTHUMBNAILER_INSTALLED)
FILE_LISTS_$(CONFIG_LIBFFMPEGTHUMBNAILER) += libffmpegthumbnailer.lst

CLEAN_RULES += clean-libffmpegthumbnailer
DISTCLEAN_RULES += distclean-libffmpegthumbnailer

#
# download libffmpegthumbnailer
#

$(LIBFFMPEGTHUMBNAILER_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBFFMPEGTHUMBNAILER_DLFILE) ] ; then \
	$(WGET) $(LIBFFMPEGTHUMBNAILER_URL) -O $(LIBFFMPEGTHUMBNAILER_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBFFMPEGTHUMBNAILER_DLFILE)

#
# unpack libffmpegthumbnailer
#

$(STAGEFILES_DIR)/.libffmpegthumbnailer_unpacked: $(LIBFFMPEGTHUMBNAILER_DLFILE) \
                                           $(wildcard $(LIBFFMPEGTHUMBNAILER_PATCHES_DIR)/*.patch) \
                                           $$(LIBFFMPEGTHUMBNAILER_DEPS)
	-$(RM) -rf $(LIBFFMPEGTHUMBNAILER_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBFFMPEGTHUMBNAILER_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libffmpegthumbnailer_unpacked

#
# patch libffmpegthumbnailer
#

$(STAGEFILES_DIR)/.libffmpegthumbnailer_patched: $(STAGEFILES_DIR)/.libffmpegthumbnailer_unpacked
	$(call patch_package, $(LIBFFMPEGTHUMBNAILER_DIR), $(LIBFFMPEGTHUMBNAILER_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libffmpegthumbnailer_patched

#
# configure libffmpegthumbnailer
#

$(STAGEFILES_DIR)/.libffmpegthumbnailer_configured: $(STAGEFILES_DIR)/.libffmpegthumbnailer_patched
	($(CD) $(LIBFFMPEGTHUMBNAILER_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="-L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(LIBFFMPEGTHUMBNAILER_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libffmpegthumbnailer_configured

#
# compile libffmpegthumbnailer
#

$(STAGEFILES_DIR)/.libffmpegthumbnailer_compiled: $(STAGEFILES_DIR)/.libffmpegthumbnailer_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBFFMPEGTHUMBNAILER_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libffmpegthumbnailer_compiled

#
# install libffmpegthumbnailer
#

$(STAGEFILES_DIR)/.libffmpegthumbnailer_installed: $(STAGEFILES_DIR)/.libffmpegthumbnailer_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBFFMPEGTHUMBNAILER_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libffmpegthumbnailer_installed


$(FILELIST_DIR)/libffmpegthumbnailer.lst: $(STAGEFILES_DIR)/.libffmpegthumbnailer_installed
	$(TOUCH) $(FILELIST_DIR)/libffmpegthumbnailer.lst

.PHONY: clean-libffmpegthumbnailer distclean-libffmpegthumbnailer

clean-libffmpegthumbnailer:
	-$(RM) -rf $(LIBFFMPEGTHUMBNAILER_DIR)

#
# clean everthing else
#

distclean-libffmpegthumbnailer:
	-$(RM) -f $(STAGEFILES_DIR)/.libffmpegthumbnailer_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libffmpegthumbnailer_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libffmpegthumbnailer_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libffmpegthumbnailer_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libffmpegthumbnailer_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBFFMPEGTHUMBNAILER_DLFILE)
endif
