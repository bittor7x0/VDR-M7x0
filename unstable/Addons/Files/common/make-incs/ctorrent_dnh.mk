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
CTORRENT_DNH_DEPS = $(BASE_BUILD_STAGEFILE)

CTORRENT_DNH_VERSION := 3.3
CTORRENT_DNH_PATCHES_DIR := $(PATCHES_DIR)/ctorrent_dnh/$(CTORRENT_DNH_VERSION)

#http://dfn.dl.sourceforge.net/sourceforge/dtorrent/ctorrent-dnh3.3.tar.gz
CTORRENT_DNH_FILE := ctorrent-dnh$(CTORRENT_DNH_VERSION).tar.gz
CTORRENT_DNH_DLFILE := $(DOWNLOAD_DIR)/$(CTORRENT_DNH_FILE)
CTORRENT_DNH_URL := http://dfn.dl.sourceforge.net/sourceforge/dtorrent/$(CTORRENT_DNH_FILE)
CTORRENT_DNH_DIR := $(BUILD_DIR)/ctorrent-dnh$(CTORRENT_DNH_VERSION)

CTORRENT_DNH_INSTALLED = $(STAGEFILES_DIR)/.ctorrent_dnh_installed

PACKS_RULES_$(CONFIG_CTORRENT_DNH) += $(CTORRENT_DNH_INSTALLED)
FILE_LISTS_$(CONFIG_CTORRENT_DNH) += ctorrent_dnh.lst

CLEAN_RULES += clean-ctorrent_dnh
DISTCLEAN_RULES += distclean-ctorrent_dnh

#
# download ctorrent_dnh
#

$(CTORRENT_DNH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CTORRENT_DNH_DLFILE) ] ; then \
	$(WGET) $(CTORRENT_DNH_URL) -O $(CTORRENT_DNH_DLFILE) ; \
	fi );
	$(TOUCH) $(CTORRENT_DNH_DLFILE)

#
# unpack ctorrent_dnh
#

$(STAGEFILES_DIR)/.ctorrent_dnh_unpacked: $(CTORRENT_DNH_DLFILE) \
                                           $(wildcard $(CTORRENT_DNH_PATCHES_DIR)/*.patch) \
                                           $$(CTORRENT_DNH_DEPS)
	-$(RM) -rf $(CTORRENT_DNH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(CTORRENT_DNH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.ctorrent_dnh_unpacked

#
# patch ctorrent_dnh
#

$(STAGEFILES_DIR)/.ctorrent_dnh_patched: $(STAGEFILES_DIR)/.ctorrent_dnh_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(CTORRENT_DNH_DIR) $(CTORRENT_DNH_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ctorrent_dnh_patched

#
# configure ctorrent_dnh
#

$(STAGEFILES_DIR)/.ctorrent_dnh_configured: $(STAGEFILES_DIR)/.ctorrent_dnh_patched
	($(CD) $(CTORRENT_DNH_DIR) ; $(UCLIBC_ENV) \
		$(CTORRENT_DNH_DIR)/configure \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.ctorrent_dnh_configured

#
# compile ctorrent_dnh
#

$(STAGEFILES_DIR)/.ctorrent_dnh_compiled: $(STAGEFILES_DIR)/.ctorrent_dnh_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(CTORRENT_DNH_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ctorrent_dnh_compiled

#
# install ctorrent_dnh
#

$(STAGEFILES_DIR)/.ctorrent_dnh_installed: $(STAGEFILES_DIR)/.ctorrent_dnh_compiled
	$(CP) $(CTORRENT_DNH_DIR)/ctorrent $(TARGET_ROOT)/usr/sbin/ctorrent
	chmod 755 $(TARGET_ROOT)/usr/sbin/ctorrent
	$(TOUCH) $(STAGEFILES_DIR)/.ctorrent_dnh_installed


$(FILELIST_DIR)/ctorrent_dnh.lst: $(STAGEFILES_DIR)/.ctorrent_dnh_installed
	$(TOUCH) $(FILELIST_DIR)/ctorrent_dnh.lst

.PHONY: clean-ctorrent_dnh distclean-ctorrent_dnh

clean-ctorrent_dnh:
	-$(RM) -rf $(CTORRENT_DNH_DIR)

#
# clean everthing else
#

distclean-ctorrent_dnh:
	-$(RM) -f $(STAGEFILES_DIR)/.ctorrent_dnh_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ctorrent_dnh_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ctorrent_dnh_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ctorrent_dnh_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ctorrent_dnh_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CTORRENT_DNH_DLFILE)
endif
