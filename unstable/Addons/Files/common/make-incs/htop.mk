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
HTOP_DEPS = $(BASE_BUILD_STAGEFILE) $(NCURSES_INSTALLED)

ifeq ($(CONFIG_HTOP),y)
ifneq ($(CONFIG_NCURSES),y)
   $(error dependency error: htop needs ncurses enabled)
endif
endif

HTOP_VERSION := 0.8.1
HTOP_PATCHES_DIR := $(PATCHES_DIR)/htop/$(HTOP_VERSION)

HTOP_FILE := htop-$(HTOP_VERSION).tar.gz
HTOP_DLFILE := $(DOWNLOAD_DIR)/$(HTOP_FILE)
HTOP_URL := http://ovh.dl.sourceforge.net/sourceforge/htop/$(HTOP_FILE)
HTOP_DIR := $(BUILD_DIR)/htop-$(HTOP_VERSION)

HTOP_INSTALLED = $(STAGEFILES_DIR)/.htop_installed

PACKS_RULES_$(CONFIG_HTOP) += $(HTOP_INSTALLED)
FILE_LISTS_$(CONFIG_HTOP) += htop.lst

CLEAN_RULES += clean-htop
DISTCLEAN_RULES += distclean-htop

#
# download htop
#

$(HTOP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(HTOP_DLFILE) ] ; then \
	$(WGET) $(HTOP_URL) -O $(HTOP_DLFILE) ; \
	fi );
	$(TOUCH) $(HTOP_DLFILE)

#
# unpack htop
#

$(STAGEFILES_DIR)/.htop_unpacked: $(HTOP_DLFILE) \
                                           $(wildcard $(HTOP_PATCHES_DIR)/*.patch) \
                                           $$(HTOP_DEPS)
	-$(RM) -rf $(HTOP_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(HTOP_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.htop_unpacked

#
# patch htop
#

$(STAGEFILES_DIR)/.htop_patched: $(STAGEFILES_DIR)/.htop_unpacked
	$(call patch_package, $(HTOP_DIR), $(HTOP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.htop_patched

#
# configure htop
#

$(STAGEFILES_DIR)/.htop_configured: $(STAGEFILES_DIR)/.htop_patched
	($(CD) $(HTOP_DIR) ; export ac_cv_func_malloc_0_nonnull=yes ; export ac_cv_func_realloc_0_nonnull=yes; \
		$(UCLIBC_ENV) \
		$(HTOP_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--build=$(HOST_TARGET) \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.htop_configured

#
# compile htop
#

$(STAGEFILES_DIR)/.htop_compiled: $(STAGEFILES_DIR)/.htop_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(HTOP_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.htop_compiled

#
# install htop
#

$(STAGEFILES_DIR)/.htop_installed: $(STAGEFILES_DIR)/.htop_compiled
	$(CP) $(HTOP_DIR)/htop $(TARGET_ROOT)/usr/bin/htop
	chmod 755 $(TARGET_ROOT)/usr/bin/htop
	$(TOUCH) $(STAGEFILES_DIR)/.htop_installed


$(FILELIST_DIR)/htop.lst: $(STAGEFILES_DIR)/.htop_installed
	$(TOUCH) $(FILELIST_DIR)/htop.lst

.PHONY: clean-htop distclean-htop

clean-htop:
	-$(RM) -rf $(HTOP_DIR)

#
# clean everthing else
#

distclean-htop:
	-$(RM) -f $(STAGEFILES_DIR)/.htop_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.htop_patched
	-$(RM) -f $(STAGEFILES_DIR)/.htop_configured
	-$(RM) -f $(STAGEFILES_DIR)/.htop_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.htop_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(HTOP_DLFILE)
endif
