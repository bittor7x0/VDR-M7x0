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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBEXIF_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_LIBICONV),y)
	LIBEXIF_DEPS +=  $(LIBICONV_INSTALLED)
endif

LIBEXIF_VERSION := 0.6.19
LIBEXIF_PATCHES_DIR := $(PATCHES_DIR)/libexif/$(LIBEXIF_VERSION)

LIBEXIF_FILE := libexif-$(LIBEXIF_VERSION).tar.bz2
LIBEXIF_DLFILE := $(DOWNLOAD_DIR)/$(LIBEXIF_FILE)
LIBEXIF_URL := http://downloads.sourceforge.net/libexif/$(LIBEXIF_FILE)
LIBEXIF_DIR := $(BUILD_DIR)/libexif-$(LIBEXIF_VERSION)

LIBEXIF_INSTALLED = $(STAGEFILES_DIR)/.libexif_installed

PACKS_RULES_$(CONFIG_LIBEXIF) += $(LIBEXIF_INSTALLED)
FILE_LISTS_$(CONFIG_LIBEXIF) += libexif.lst

CLEAN_RULES += clean-libexif
DISTCLEAN_RULES += distclean-libexif

#
# download libexif
#

$(LIBEXIF_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBEXIF_DLFILE) ] ; then \
	$(WGET) $(LIBEXIF_URL) -O $(LIBEXIF_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBEXIF_DLFILE)

#
# unpack libexif
#

$(STAGEFILES_DIR)/.libexif_unpacked: $(LIBEXIF_DLFILE) \
                                           $(wildcard $(LIBEXIF_PATCHES_DIR)/*.patch) \
                                           $$(LIBEXIF_DEPS)
	-$(RM) -rf $(LIBEXIF_DIR)
	$(BZCAT) $(LIBEXIF_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.libexif_unpacked

#
# patch libexif
#

$(STAGEFILES_DIR)/.libexif_patched: $(STAGEFILES_DIR)/.libexif_unpacked
	$(call patch_package, $(LIBEXIF_DIR), $(LIBEXIF_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libexif_patched

#
# configure libexif
#

$(STAGEFILES_DIR)/.libexif_configured: $(STAGEFILES_DIR)/.libexif_patched
	($(CD) $(LIBEXIF_DIR) ; $(UCLIBC_ENV) \
		$(LIBEXIF_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--disable-docs \
			--without-libintl-prefix \
			$(if $(CONFIG_LIBICONV),--with-libiconv-prefix=$(TARGET_ROOT)/usr/lib/libiconv,--without-libiconv-prefix))
	$(TOUCH) $(STAGEFILES_DIR)/.libexif_configured

#
# compile libexif
#

$(STAGEFILES_DIR)/.libexif_compiled: $(STAGEFILES_DIR)/.libexif_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBEXIF_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libexif_compiled

#
# install libexif
#

$(STAGEFILES_DIR)/.libexif_installed: $(STAGEFILES_DIR)/.libexif_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBEXIF_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libexif_installed


$(FILELIST_DIR)/libexif.lst: $(STAGEFILES_DIR)/.libexif_installed
	$(TOUCH) $(FILELIST_DIR)/libexif.lst

.PHONY: clean-libexif distclean-libexif

clean-libexif:
	-$(RM) -rf $(LIBEXIF_DIR)

#
# clean everthing else
#

distclean-libexif:
	-$(RM) -f $(STAGEFILES_DIR)/.libexif_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libexif_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libexif_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libexif_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libexif_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBEXIF_DLFILE)
endif
