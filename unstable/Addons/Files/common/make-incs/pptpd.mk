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
# The project's page is at http://www.assembla.com/spaces/VDR-M7x0
#
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
PPTPD_DEPS = $(BASE_BUILD_STAGEFILE)

PPTPD_VERSION := 1.3.0
PPTPD_PATCHES_DIR := $(PATCHES_DIR)/pptpd/$(PPTPD_VERSION)

PPTPD_FILE := pptpd-$(PPTPD_VERSION).tar.gz
PPTPD_DLFILE := $(DOWNLOAD_DIR)/$(PPTPD_FILE)
PPTPD_URL := http://downloads.sourceforge.net/poptop/$(PPTPD_FILE)
PPTPD_DIR := $(BUILD_DIR)/pptpd-$(PPTPD_VERSION)

PPTPD_INSTALLED = $(STAGEFILES_DIR)/.pptpd_installed

PACKS_RULES_$(CONFIG_PPTPD) += $(PPTPD_INSTALLED)
FILE_LISTS_$(CONFIG_PPTPD) += pptpd.lst

CLEAN_RULES += clean-pptpd
DISTCLEAN_RULES += distclean-pptpd

#
# download pptpd
#

$(PPTPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PPTPD_DLFILE) ] ; then \
	$(WGET) $(PPTPD_URL) -O $(PPTPD_DLFILE) ; \
	fi );
	$(TOUCH) $(PPTPD_DLFILE)

#
# unpack pptpd
#

$(STAGEFILES_DIR)/.pptpd_unpacked: $(PPTPD_DLFILE) \
                                           $(wildcard $(PPTPD_PATCHES_DIR)/*.patch) \
                                           $$(PPTPD_DEPS)
	-$(RM) -rf $(PPTPD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(PPTPD_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.pptpd_unpacked

#
# patch pptpd
#

$(STAGEFILES_DIR)/.pptpd_patched: $(STAGEFILES_DIR)/.pptpd_unpacked
	$(call patch_package, $(PPTPD_DIR), $(PPTPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.pptpd_patched

#
# configure pptpd
#

$(STAGEFILES_DIR)/.pptpd_configured: $(STAGEFILES_DIR)/.pptpd_patched
	($(CD) $(PPTPD_DIR) ; $(UCLIBC_ENV) \
		$(PPTPD_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.pptpd_configured

#
# compile pptpd
#

$(STAGEFILES_DIR)/.pptpd_compiled: $(STAGEFILES_DIR)/.pptpd_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(PPTPD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.pptpd_compiled

#
# install pptpd
#

$(STAGEFILES_DIR)/.pptpd_installed: $(STAGEFILES_DIR)/.pptpd_compiled
	$(CP) $(PPTPD_DIR)/pptpctrl $(TARGET_ROOT)/usr/sbin/pptpctrl
	chmod 755 $(TARGET_ROOT)/usr/sbin/pptpctrl
	$(MKDIR) -p $(TARGET_ROOT)/etc/ppp
	$(CP) $(PPTPD_DIR)/samples/options.pptpd $(TARGET_ROOT)/etc/ppp/options
	$(CP) $(PPTPD_DIR)/samples/chap-secrets $(TARGET_ROOT)/etc/ppp/chap-secrets
	(if [ X"`$(GREP) pptpctrl $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services`" = X"" ] ; then \
		$(SED) -i '$$apptpctrl\t1723/tcp' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services ; \
	fi);
	(if [ X"`$(GREP) pptpctrl $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf`" = X"" ] ; then \
		$(SED) -i '$$apptpctrl\tstream\ttcp\tnowait\troot\t/usr/sbin/pptpctrl ReservedToWriteCorrectNameAndAddressOfPeer 0 0 0 0 0 0 0' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf ; \
	fi);
	$(TOUCH) $(STAGEFILES_DIR)/.pptpd_installed


$(FILELIST_DIR)/pptpd.lst: $(STAGEFILES_DIR)/.pptpd_installed
	$(TOUCH) $(FILELIST_DIR)/pptpd.lst

.PHONY: clean-pptpd distclean-pptpd

clean-pptpd:
	-$(RM) -rf $(PPTPD_DIR)

#
# clean everthing else
#

distclean-pptpd:
	-$(RM) -f $(STAGEFILES_DIR)/.pptpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.pptpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.pptpd_configured
	-$(RM) -f $(STAGEFILES_DIR)/.pptpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.pptpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PPTPD_DLFILE)
endif
