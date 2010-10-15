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

ifeq ($(CONFIG_LIBDLNA),y)
ifneq ($(CONFIG_FFMPEG),y)
   $(error dependency error: libdlna needs ffmpeg enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBDLNA_DEPS = $(BASE_BUILD_STAGEFILE) $(FFMPEG_INSTALLED)

LIBDLNA_VERSION := 0.2.3
LIBDLNA_PATCHES_DIR := $(PATCHES_DIR)/libdlna/$(LIBDLNA_VERSION)

LIBDLNA_FILE := libdlna-$(LIBDLNA_VERSION).tar.bz2
LIBDLNA_DLFILE := $(DOWNLOAD_DIR)/$(LIBDLNA_FILE)
LIBDLNA_URL := http://libdlna.geexbox.org/releases/$(LIBDLNA_FILE)
LIBDLNA_DIR := $(BUILD_DIR)/libdlna-$(LIBDLNA_VERSION)

LIBDLNA_INSTALLED = $(STAGEFILES_DIR)/.libdlna_installed

PACKS_RULES_$(CONFIG_LIBDLNA) += $(LIBDLNA_INSTALLED)
FILE_LISTS_$(CONFIG_LIBDLNA) += libdlna.lst

CLEAN_RULES += clean-libdlna
DISTCLEAN_RULES += distclean-libdlna

#
# download libdlna
#

$(LIBDLNA_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBDLNA_DLFILE) ] ; then \
	$(WGET) $(LIBDLNA_URL) -O $(LIBDLNA_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBDLNA_DLFILE)

#
# unpack libdlna
#

$(STAGEFILES_DIR)/.libdlna_unpacked: $(LIBDLNA_DLFILE) \
                                           $(wildcard $(LIBDLNA_PATCHES_DIR)/*.patch) \
                                           $$(LIBDLNA_DEPS)
	-$(RM) -rf $(LIBDLNA_DIR)
	$(BZCAT) $(LIBDLNA_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.libdlna_unpacked

#
# patch libdlna
#

$(STAGEFILES_DIR)/.libdlna_patched: $(STAGEFILES_DIR)/.libdlna_unpacked
	$(call patch_package, $(LIBDLNA_DIR), $(LIBDLNA_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libdlna_patched

#
# configure libdlna (this is *NOT* GNU configure)
# 

$(STAGEFILES_DIR)/.libdlna_configured: $(STAGEFILES_DIR)/.libdlna_patched
	($(CD) $(LIBDLNA_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="-L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib -lpthread" \
		HOST_CC="$(CC)" \
		$(LIBDLNA_DIR)/configure \
		--cross-compile \
		--cross-prefix="$(UCLIBC_TARGET)-" \
		--prefix=$(TARGET_ROOT)/usr \
		--enable-shared \
		--enable-static \
		--disable-debug \
		--disable-optimize \
		--disable-strip)
	$(TOUCH) $(STAGEFILES_DIR)/.libdlna_configured

#
# compile libdlna
#

$(STAGEFILES_DIR)/.libdlna_compiled: $(STAGEFILES_DIR)/.libdlna_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBDLNA_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.libdlna_compiled

#
# install libdlna
#

$(STAGEFILES_DIR)/.libdlna_installed: $(STAGEFILES_DIR)/.libdlna_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBDLNA_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libdlna_installed


$(FILELIST_DIR)/libdlna.lst: $(STAGEFILES_DIR)/.libdlna_installed
	$(TOUCH) $(FILELIST_DIR)/libdlna.lst

.PHONY: clean-libdlna distclean-libdlna

clean-libdlna:
	-$(RM) -rf $(LIBDLNA_DIR)

#
# clean everthing else
#

distclean-libdlna:
	-$(RM) -f $(STAGEFILES_DIR)/.libdlna_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libdlna_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libdlna_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libdlna_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libdlna_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBDLNA_DLFILE)
endif
