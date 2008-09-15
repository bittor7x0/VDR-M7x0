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
DVBSNOOP_DEPS = $(BASE_BUILD_STAGEFILE)

DVBSNOOP_VERSION := 1.4.50
DVBSNOOP_PATCHES_DIR := $(PATCHES_DIR)/dvbsnoop/$(DVBSNOOP_VERSION)

#http://easynews.dl.sourceforge.net/sourceforge/dvbsnoop/dvbsnoop-1.4.50.tar.gz
DVBSNOOP_FILE := dvbsnoop-$(DVBSNOOP_VERSION).tar.gz
DVBSNOOP_DLFILE := $(DOWNLOAD_DIR)/$(DVBSNOOP_FILE)
DVBSNOOP_URL := http://easynews.dl.sourceforge.net/sourceforge/dvbsnoop/$(DVBSNOOP_FILE)
DVBSNOOP_DIR := $(BUILD_DIR)/dvbsnoop-$(DVBSNOOP_VERSION)

DVBSNOOP_INSTALLED = $(STAGEFILES_DIR)/.dvbsnoop_installed

PACKS_RULES_$(CONFIG_DVBSNOOP) += $(DVBSNOOP_INSTALLED)
FILE_LISTS_$(CONFIG_DVBSNOOP) += dvbsnoop.lst

CLEAN_RULES += clean-dvbsnoop
DISTCLEAN_RULES += distclean-dvbsnoop

#
# download dvbsnoop
#

$(DVBSNOOP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DVBSNOOP_DLFILE) ] ; then \
	$(WGET) $(DVBSNOOP_URL) -O $(DVBSNOOP_DLFILE) ; \
	fi );
	$(TOUCH) $(DVBSNOOP_DLFILE)

#
# unpack dvbsnoop
#

$(STAGEFILES_DIR)/.dvbsnoop_unpacked: $(DVBSNOOP_DLFILE) \
                                           $(wildcard $(DVBSNOOP_PATCHES_DIR)/*.patch) \
                                           $$(DVBSNOOP_DEPS)
	-$(RM) -rf $(DVBSNOOP_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(DVBSNOOP_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.dvbsnoop_unpacked

#
# patch dvbsnoop
#

$(STAGEFILES_DIR)/.dvbsnoop_patched: $(STAGEFILES_DIR)/.dvbsnoop_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(DVBSNOOP_DIR) $(DVBSNOOP_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dvbsnoop_patched

#
# configure dvbsnoop
#

$(STAGEFILES_DIR)/.dvbsnoop_configured: $(STAGEFILES_DIR)/.dvbsnoop_patched
	($(CD) $(DVBSNOOP_DIR) ; $(UCLIBC_ENV) \
		$(DVBSNOOP_DIR)/configure \
			--host=$(TARGET) \
			CPPFLAGS=-I/usr/src/linux-headers-`uname -r`/include)
	$(TOUCH) $(STAGEFILES_DIR)/.dvbsnoop_configured

#
# compile dvbsnoop
#

$(STAGEFILES_DIR)/.dvbsnoop_compiled: $(STAGEFILES_DIR)/.dvbsnoop_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(DVBSNOOP_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dvbsnoop_compiled

#
# install dvbsnoop
#

$(STAGEFILES_DIR)/.dvbsnoop_installed: $(STAGEFILES_DIR)/.dvbsnoop_compiled
	$(CP) $(DVBSNOOP_DIR)/src/dvbsnoop $(TARGET_ROOT)/usr/sbin/dvbsnoop
	chmod 755 $(TARGET_ROOT)/usr/sbin/dvbsnoop
	$(TOUCH) $(STAGEFILES_DIR)/.dvbsnoop_installed


$(FILELIST_DIR)/dvbsnoop.lst: $(STAGEFILES_DIR)/.dvbsnoop_installed
	$(TOUCH) $(FILELIST_DIR)/dvbsnoop.lst

.PHONY: clean-dvbsnoop distclean-dvbsnoop

clean-dvbsnoop:
	-$(RM) -rf $(DVBSNOOP_DIR)

#
# clean everthing else
#

distclean-dvbsnoop:
	-$(RM) -f $(STAGEFILES_DIR)/.dvbsnoop_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dvbsnoop_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dvbsnoop_configured
	-$(RM) -f $(STAGEFILES_DIR)/.dvbsnoop_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dvbsnoop_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DVBSNOOP_DLFILE)
endif
