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
LIBJPEG_DEPS = $(BASE_BUILD_STAGEFILE)

LIBJPEG_VERSION := 8b
LIBJPEG_PATCHES_DIR := $(PATCHES_DIR)/libjpeg/$(LIBJPEG_VERSION)

LIBJPEG_FILE := jpegsrc.v$(LIBJPEG_VERSION).tar.gz
LIBJPEG_DLFILE := $(DOWNLOAD_DIR)/$(LIBJPEG_FILE)
LIBJPEG_URL := http://www.ijg.org/files/$(LIBJPEG_FILE)
LIBJPEG_DIR := $(BUILD_DIR)/jpeg-$(LIBJPEG_VERSION)

LIBJPEG_INSTALLED = $(STAGEFILES_DIR)/.libjpeg_installed

PACKS_RULES_$(CONFIG_LIBJPEG) += $(LIBJPEG_INSTALLED)
FILE_LISTS_$(CONFIG_LIBJPEG) += libjpeg.lst

CLEAN_RULES += clean-libjpeg
DISTCLEAN_RULES += distclean-libjpeg

#
# download libjpeg
#

$(LIBJPEG_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBJPEG_DLFILE) ] ; then \
	$(WGET) $(LIBJPEG_URL) -O $(LIBJPEG_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBJPEG_DLFILE)

#
# unpack libjpeg
#

$(STAGEFILES_DIR)/.libjpeg_unpacked: $(LIBJPEG_DLFILE) \
                                           $(wildcard $(LIBJPEG_PATCHES_DIR)/*.patch) \
                                           $$(LIBJPEG_DEPS)
	-$(RM) -rf $(LIBJPEG_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBJPEG_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libjpeg_unpacked

#
# patch libjpeg
#

$(STAGEFILES_DIR)/.libjpeg_patched: $(STAGEFILES_DIR)/.libjpeg_unpacked
	$(call patch_package, $(LIBJPEG_DIR), $(LIBJPEG_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libjpeg_patched

#
# configure libjpeg
#

$(STAGEFILES_DIR)/.libjpeg_configured: $(STAGEFILES_DIR)/.libjpeg_patched
	($(CD) $(LIBJPEG_DIR) ; $(UCLIBC_ENV) \
		$(LIBJPEG_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libjpeg_configured

#
# compile libjpeg
#

$(STAGEFILES_DIR)/.libjpeg_compiled: $(STAGEFILES_DIR)/.libjpeg_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBJPEG_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.libjpeg_compiled

#
# install libjpeg
#

$(STAGEFILES_DIR)/.libjpeg_installed: $(STAGEFILES_DIR)/.libjpeg_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBJPEG_DIR) install
	$(LN) -sf $(TARGET_ROOT)/usr/include/libjpeg2/libjpeg $(TARGET_ROOT)/usr/include/libjpeg
	$(TOUCH) $(STAGEFILES_DIR)/.libjpeg_installed


$(FILELIST_DIR)/libjpeg.lst: $(STAGEFILES_DIR)/.libjpeg_installed
	$(TOUCH) $(FILELIST_DIR)/libjpeg.lst

.PHONY: clean-libjpeg distclean-libjpeg

clean-libjpeg:
	-$(RM) -rf $(LIBJPEG_DIR)

#
# clean everthing else
#

distclean-libjpeg:
	-$(RM) -f $(STAGEFILES_DIR)/.libjpeg_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libjpeg_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libjpeg_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libjpeg_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libjpeg_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBJPEG_DLFILE)
endif
