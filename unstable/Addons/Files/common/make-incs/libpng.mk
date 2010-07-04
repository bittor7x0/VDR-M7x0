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
LIBPNG_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

LIBPNG_VERSION := 1.4.2
LIBPNG_PATCHES_DIR := $(PATCHES_DIR)/libpng/$(LIBPNG_VERSION)

LIBPNG_FILE := libpng-$(LIBPNG_VERSION).tar.bz2
LIBPNG_DLFILE := $(DOWNLOAD_DIR)/$(LIBPNG_FILE)
LIBPNG_URL := http://sourceforge.net/projects/libpng/files/01-libpng-master/$(LIBPNG_VERSION)/$(LIBPNG_FILE)/download
LIBPNG_DIR := $(BUILD_DIR)/libpng-$(LIBPNG_VERSION)

LIBPNG_INSTALLED = $(STAGEFILES_DIR)/.libpng_installed

PACKS_RULES_$(CONFIG_LIBPNG) += $(LIBPNG_INSTALLED)
FILE_LISTS_$(CONFIG_LIBPNG) += libpng.lst

CLEAN_RULES += clean-libpng
DISTCLEAN_RULES += distclean-libpng

#
# download libpng
#

$(LIBPNG_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBPNG_DLFILE) ] ; then \
	$(WGET) $(LIBPNG_URL) -O $(LIBPNG_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBPNG_DLFILE)

#
# unpack libpng
#

$(STAGEFILES_DIR)/.libpng_unpacked: $(LIBPNG_DLFILE) \
                                           $(wildcard $(LIBPNG_PATCHES_DIR)/*.patch) \
                                           $$(LIBPNG_DEPS)
	-$(RM) -rf $(LIBPNG_DIR)
	$(BZCAT) $(LIBPNG_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.libpng_unpacked

#
# patch libpng
#

$(STAGEFILES_DIR)/.libpng_patched: $(STAGEFILES_DIR)/.libpng_unpacked
	$(call patch_package, $(LIBPNG_DIR), $(LIBPNG_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libpng_patched

#
# configure libpng
#

$(STAGEFILES_DIR)/.libpng_configured: $(STAGEFILES_DIR)/.libpng_patched
	($(CD) $(LIBPNG_DIR) ; $(UCLIBC_ENV) \
		$(LIBPNG_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libpng_configured

#
# compile libpng
#

$(STAGEFILES_DIR)/.libpng_compiled: $(STAGEFILES_DIR)/.libpng_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBPNG_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.libpng_compiled

#
# install libpng
#

$(STAGEFILES_DIR)/.libpng_installed: $(STAGEFILES_DIR)/.libpng_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBPNG_DIR) install
	$(LN) -sf $(TARGET_ROOT)/usr/include/libpng2/libpng $(TARGET_ROOT)/usr/include/libpng
	$(TOUCH) $(STAGEFILES_DIR)/.libpng_installed


$(FILELIST_DIR)/libpng.lst: $(STAGEFILES_DIR)/.libpng_installed
	$(TOUCH) $(FILELIST_DIR)/libpng.lst

.PHONY: clean-libpng distclean-libpng

clean-libpng:
	-$(RM) -rf $(LIBPNG_DIR)

#
# clean everthing else
#

distclean-libpng:
	-$(RM) -f $(STAGEFILES_DIR)/.libpng_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libpng_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libpng_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libpng_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libpng_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBPNG_DLFILE)
endif
