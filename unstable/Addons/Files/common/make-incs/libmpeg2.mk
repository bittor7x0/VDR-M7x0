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
LIBMPEG2_DEPS = $(BASE_BUILD_STAGEFILE)

LIBMPEG2_VERSION := 0.4.1
LIBMPEG2_PATCHES_DIR := $(PATCHES_DIR)/libmpeg2/$(LIBMPEG2_VERSION)

LIBMPEG2_FILE := mpeg2dec-$(LIBMPEG2_VERSION).tar.gz
LIBMPEG2_DLFILE := $(DOWNLOAD_DIR)/$(LIBMPEG2_FILE)
LIBMPEG2_URL := http://libmpeg2.sourceforge.net/files/$(LIBMPEG2_FILE)
LIBMPEG2_DIR := $(BUILD_DIR)/mpeg2dec-$(LIBMPEG2_VERSION)

LIBMPEG2_INSTALLED = $(STAGEFILES_DIR)/.libmpeg2_installed

PACKS_RULES_$(CONFIG_LIBMPEG2) += $(LIBMPEG2_INSTALLED)
FILE_LISTS_$(CONFIG_LIBMPEG2) += libmpeg2.lst

CLEAN_RULES += clean-libmpeg2
DISTCLEAN_RULES += distclean-libmpeg2

#
# download libmpeg2
#

$(LIBMPEG2_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBMPEG2_DLFILE) ] ; then \
	$(WGET) $(LIBMPEG2_URL) -O $(LIBMPEG2_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBMPEG2_DLFILE)

#
# unpack libmpeg2
#

$(STAGEFILES_DIR)/.libmpeg2_unpacked: $(LIBMPEG2_DLFILE) \
                                           $(wildcard $(LIBMPEG2_PATCHES_DIR)/*.patch) \
                                           $$(LIBMPEG2_DEPS)
	-$(RM) -rf $(LIBMPEG2_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBMPEG2_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libmpeg2_unpacked

#
# patch libmpeg2
#

$(STAGEFILES_DIR)/.libmpeg2_patched: $(STAGEFILES_DIR)/.libmpeg2_unpacked
	$(call patch_package, $(LIBMPEG2_DIR), $(LIBMPEG2_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libmpeg2_patched

#
# configure libmpeg2
#

$(STAGEFILES_DIR)/.libmpeg2_configured: $(STAGEFILES_DIR)/.libmpeg2_patched
	($(CD) $(LIBMPEG2_DIR) ; $(UCLIBC_ENV) \
		$(LIBMPEG2_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libmpeg2_configured

#
# compile libmpeg2
#

$(STAGEFILES_DIR)/.libmpeg2_compiled: $(STAGEFILES_DIR)/.libmpeg2_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBMPEG2_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libmpeg2_compiled

#
# install libmpeg2
#

$(STAGEFILES_DIR)/.libmpeg2_installed: $(STAGEFILES_DIR)/.libmpeg2_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBMPEG2_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libmpeg2_installed


$(FILELIST_DIR)/libmpeg2.lst: $(STAGEFILES_DIR)/.libmpeg2_installed
	$(TOUCH) $(FILELIST_DIR)/libmpeg2.lst

.PHONY: clean-libmpeg2 distclean-libmpeg2

clean-libmpeg2:
	-$(RM) -rf $(LIBMPEG2_DIR)

#
# clean everthing else
#

distclean-libmpeg2:
	-$(RM) -f $(STAGEFILES_DIR)/.libmpeg2_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libmpeg2_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libmpeg2_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libmpeg2_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libmpeg2_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBMPEG2_DLFILE)
endif
