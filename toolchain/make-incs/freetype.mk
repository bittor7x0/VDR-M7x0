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
FREETYPE_DEPS = $(BASE_BUILD_STAGEFILE)

FREETYPE_VERSION := 2.3.9
FREETYPE_PATCHES_DIR := $(PATCHES_DIR)/freetype/$(FREETYPE_VERSION)

FREETYPE_FILE := freetype-$(FREETYPE_VERSION).tar.bz2
FREETYPE_DLFILE := $(DOWNLOAD_DIR)/$(FREETYPE_FILE)
FREETYPE_URL := http://sourceforge.net/projects/freetype/files/freetype2/$(FREETYPE_VERSION)/$(FREETYPE_FILE)/download
FREETYPE_DIR := $(BUILD_DIR)/freetype-$(FREETYPE_VERSION)

FREETYPE_INSTALLED = $(STAGEFILES_DIR)/.freetype_installed

PACKS_RULES_$(CONFIG_FREETYPE) += $(FREETYPE_INSTALLED)
FILE_LISTS_$(CONFIG_FREETYPE) += freetype.lst

CLEAN_RULES += clean-freetype
DISTCLEAN_RULES += distclean-freetype

#
# download freetype
#

$(FREETYPE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(FREETYPE_DLFILE) ] ; then \
	$(WGET) $(FREETYPE_URL) -O $(FREETYPE_DLFILE) ; \
	fi );
	$(TOUCH) $(FREETYPE_DLFILE)

#
# unpack freetype
#

$(STAGEFILES_DIR)/.freetype_unpacked: $(FREETYPE_DLFILE) \
                                           $(wildcard $(FREETYPE_PATCHES_DIR)/*.patch) \
                                           $$(FREETYPE_DEPS)
	-$(RM) -rf $(FREETYPE_DIR)
	$(BZCAT) $(FREETYPE_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.freetype_unpacked

#
# patch freetype
#

$(STAGEFILES_DIR)/.freetype_patched: $(STAGEFILES_DIR)/.freetype_unpacked
	$(call patch_package, $(FREETYPE_DIR), $(FREETYPE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.freetype_patched

#
# configure freetype
#

$(STAGEFILES_DIR)/.freetype_configured: $(STAGEFILES_DIR)/.freetype_patched
	($(CD) $(FREETYPE_DIR) ; $(UCLIBC_ENV) \
		$(FREETYPE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.freetype_configured

#
# compile freetype
#

$(STAGEFILES_DIR)/.freetype_compiled: $(STAGEFILES_DIR)/.freetype_configured
	$(UCLIBC_ENV) $(MAKE) -C $(FREETYPE_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.freetype_compiled

#
# install freetype
#

$(STAGEFILES_DIR)/.freetype_installed: $(STAGEFILES_DIR)/.freetype_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(FREETYPE_DIR) install
	$(LN) -sf $(TARGET_ROOT)/usr/include/freetype2/freetype $(TARGET_ROOT)/usr/include/freetype
	$(TOUCH) $(STAGEFILES_DIR)/.freetype_installed


$(FILELIST_DIR)/freetype.lst: $(STAGEFILES_DIR)/.freetype_installed
	$(TOUCH) $(FILELIST_DIR)/freetype.lst

.PHONY: clean-freetype distclean-freetype

clean-freetype:
	-$(RM) -rf $(FREETYPE_DIR)

#
# clean everthing else
#

distclean-freetype:
	-$(RM) -f $(STAGEFILES_DIR)/.freetype_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.freetype_patched
	-$(RM) -f $(STAGEFILES_DIR)/.freetype_configured
	-$(RM) -f $(STAGEFILES_DIR)/.freetype_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.freetype_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(FREETYPE_DLFILE)
endif
