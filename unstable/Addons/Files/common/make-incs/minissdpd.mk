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
MINISSDPD_DEPS = $(BASE_BUILD_STAGEFILE)

MINISSDPD_VERSION := 1.0
MINISSDPD_PATCHES_DIR := $(PATCHES_DIR)/minissdpd/$(MINISSDPD_VERSION)

MINISSDPD_FILE := minissdpd-$(MINISSDPD_VERSION).tar.gz
MINISSDPD_DLFILE := $(DOWNLOAD_DIR)/$(MINISSDPD_FILE)
MINISSDPD_URL := http://miniupnp.free.fr/files/$(MINISSDPD_FILE)
MINISSDPD_DIR := $(BUILD_DIR)/minissdpd-$(MINISSDPD_VERSION)

MINISSDPD_INSTALLED = $(STAGEFILES_DIR)/.minissdpd_installed

PACKS_RULES_$(CONFIG_MINISSDPD) += $(MINISSDPD_INSTALLED)
FILE_LISTS_$(CONFIG_MINISSDPD) += minissdpd.lst

CLEAN_RULES += clean-minissdpd
DISTCLEAN_RULES += distclean-minissdpd

#
# download minissdpd
#

$(MINISSDPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MINISSDPD_DLFILE) ] ; then \
	$(WGET) $(MINISSDPD_URL) -O $(MINISSDPD_DLFILE) ; \
	fi );
	$(TOUCH) $(MINISSDPD_DLFILE)

#
# unpack minissdpd
#

$(STAGEFILES_DIR)/.minissdpd_unpacked: $(MINISSDPD_DLFILE) \
                                           $(wildcard $(MINISSDPD_PATCHES_DIR)/*.patch) \
                                           $$(MINISSDPD_DEPS)
	-$(RM) -rf $(MINISSDPD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(MINISSDPD_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.minissdpd_unpacked

#
# patch minissdpd
#

$(STAGEFILES_DIR)/.minissdpd_patched: $(STAGEFILES_DIR)/.minissdpd_unpacked
	$(call patch_package, $(MINISSDPD_DIR), $(MINISSDPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.minissdpd_patched

#
# compile minissdpd
#

$(STAGEFILES_DIR)/.minissdpd_compiled: $(STAGEFILES_DIR)/.minissdpd_patched
	$(UCLIBC_ENV) $(MAKE) -C $(MINISSDPD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.minissdpd_compiled

#
# install minissdpd
#

$(STAGEFILES_DIR)/.minissdpd_installed: $(STAGEFILES_DIR)/.minissdpd_compiled
	$(UCLIBC_ENV) PREFIX=$(TARGET_ROOT) \
		$(MAKE) -C $(MINISSDPD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.minissdpd_installed


$(FILELIST_DIR)/minissdpd.lst: $(STAGEFILES_DIR)/.minissdpd_installed
	$(TOUCH) $(FILELIST_DIR)/minissdpd.lst

.PHONY: clean-minissdpd distclean-minissdpd

clean-minissdpd:
	-$(RM) -rf $(MINISSDPD_DIR)

#
# clean everthing else
#

distclean-minissdpd:
	-$(RM) -f $(STAGEFILES_DIR)/.minissdpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.minissdpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.minissdpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.minissdpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MINISSDPD_DLFILE)
endif
