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
SPINDOWN_DEPS = $(BASE_BUILD_STAGEFILE)

SPINDOWN_VERSION := 1.0
SPINDOWN_PATCHES_DIR := $(PATCHES_DIR)/spindown/$(SPINDOWN_VERSION)

#http://vladimir.grouzdev.free.fr/public/spindown/src/spindown-1.0.tar.gz
SPINDOWN_FILE := spindown-$(SPINDOWN_VERSION).tar.gz
SPINDOWN_DLFILE := $(DOWNLOAD_DIR)/$(SPINDOWN_FILE)
SPINDOWN_URL := http://vladimir.grouzdev.free.fr/public/spindown/src/$(SPINDOWN_FILE)
SPINDOWN_DIR := $(BUILD_DIR)/spindown-$(SPINDOWN_VERSION)

SPINDOWN_INSTALLED = $(STAGEFILES_DIR)/.spindown_installed

PACKS_RULES_$(CONFIG_SPINDOWN) += $(SPINDOWN_INSTALLED)
FILE_LISTS_$(CONFIG_SPINDOWN) += spindown.lst

CLEAN_RULES += clean-spindown
DISTCLEAN_RULES += distclean-spindown

#
# download spindown
#

$(SPINDOWN_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SPINDOWN_DLFILE) ] ; then \
	$(WGET) $(SPINDOWN_URL) -O $(SPINDOWN_DLFILE) ; \
	fi );
	$(TOUCH) $(SPINDOWN_DLFILE)

#
# unpack spindown
#

$(STAGEFILES_DIR)/.spindown_unpacked: $(SPINDOWN_DLFILE) \
                                           $(wildcard $(SPINDOWN_PATCHES_DIR)/*.patch) \
                                           $$(SPINDOWN_DEPS)
	-$(RM) -rf $(SPINDOWN_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SPINDOWN_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.spindown_unpacked

#
# patch spindown
#

$(STAGEFILES_DIR)/.spindown_patched: $(STAGEFILES_DIR)/.spindown_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(SPINDOWN_DIR) $(SPINDOWN_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.spindown_patched

#
# configure spindown
#

$(STAGEFILES_DIR)/.spindown_configured: $(STAGEFILES_DIR)/.spindown_patched
	($(CD) $(SPINDOWN_DIR) ; $(UCLIBC_ENV) \
		$(SPINDOWN_DIR)/configure \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.spindown_configured

#
# compile spindown
#

$(STAGEFILES_DIR)/.spindown_compiled: $(STAGEFILES_DIR)/.spindown_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(SPINDOWN_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.spindown_compiled

#
# install spindown
#

$(STAGEFILES_DIR)/.spindown_installed: $(STAGEFILES_DIR)/.spindown_compiled
	$(CP) $(SPINDOWN_DIR)/src/spindown $(TARGET_ROOT)/usr/sbin/spindown
	chmod 755 $(TARGET_ROOT)/usr/sbin/spindown
	$(TOUCH) $(STAGEFILES_DIR)/.spindown_installed


$(FILELIST_DIR)/spindown.lst: $(STAGEFILES_DIR)/.spindown_installed
	$(TOUCH) $(FILELIST_DIR)/spindown.lst

.PHONY: clean-spindown distclean-spindown

clean-spindown:
	-$(RM) -rf $(SPINDOWN_DIR)

#
# clean everthing else
#

distclean-spindown:
	-$(RM) -f $(STAGEFILES_DIR)/.spindown_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.spindown_patched
	-$(RM) -f $(STAGEFILES_DIR)/.spindown_configured
	-$(RM) -f $(STAGEFILES_DIR)/.spindown_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.spindown_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SPINDOWN_DLFILE)
endif
