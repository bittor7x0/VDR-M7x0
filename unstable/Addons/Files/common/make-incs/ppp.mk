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
PPP_DEPS = $(BASE_BUILD_STAGEFILE)

PPP_VERSION := 2.4.4
PPP_PATCHES_DIR := $(PATCHES_DIR)/ppp/$(PPP_VERSION)

PPP_FILE := ppp-$(PPP_VERSION).tar.gz
PPP_DLFILE := $(DOWNLOAD_DIR)/$(PPP_FILE)
PPP_URL := ftp://ftp.samba.org/pub/ppp/$(PPP_FILE)
PPP_DIR := $(BUILD_DIR)/ppp-$(PPP_VERSION)

PPP_INSTALLED = $(STAGEFILES_DIR)/.ppp_installed

PACKS_RULES_$(CONFIG_PPP) += $(PPP_INSTALLED)
FILE_LISTS_$(CONFIG_PPP) += ppp.lst

CLEAN_RULES += clean-ppp
DISTCLEAN_RULES += distclean-ppp

#
# download ppp
#

$(PPP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PPP_DLFILE) ] ; then \
	$(WGET) $(PPP_URL) -O $(PPP_DLFILE) ; \
	fi );
	$(TOUCH) $(PPP_DLFILE)

#
# unpack ppp
#

$(STAGEFILES_DIR)/.ppp_unpacked: $(PPP_DLFILE) \
                                           $(wildcard $(PPP_PATCHES_DIR)/*.patch) \
                                           $$(PPP_DEPS)
	-$(RM) -rf $(PPP_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(PPP_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.ppp_unpacked

#
# patch ppp
#

$(STAGEFILES_DIR)/.ppp_patched: $(STAGEFILES_DIR)/.ppp_unpacked
	$(call patch package, $(PPP_DIR), $(PPP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ppp_patched

#
# configure ppp
#

$(STAGEFILES_DIR)/.ppp_configured: $(STAGEFILES_DIR)/.ppp_patched
	($(CD) $(PPP_DIR) ; $(UCLIBC_ENV) \
		$(PPP_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.ppp_configured

#
# compile ppp
#

$(STAGEFILES_DIR)/.ppp_compiled: $(STAGEFILES_DIR)/.ppp_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(PPP_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ppp_compiled

#
# install ppp
#

$(STAGEFILES_DIR)/.ppp_installed: $(STAGEFILES_DIR)/.ppp_compiled
	$(CP) $(PPP_DIR)/pppd/pppd $(TARGET_ROOT)/usr/sbin/pppd
	chmod 755 $(TARGET_ROOT)/usr/sbin/pppd
	$(TOUCH) $(STAGEFILES_DIR)/.ppp_installed


$(FILELIST_DIR)/ppp.lst: $(STAGEFILES_DIR)/.ppp_installed
	$(TOUCH) $(FILELIST_DIR)/ppp.lst

.PHONY: clean-ppp distclean-ppp

clean-ppp:
	-$(RM) -rf $(PPP_DIR)

#
# clean everthing else
#

distclean-ppp:
	-$(RM) -f $(STAGEFILES_DIR)/.ppp_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ppp_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ppp_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ppp_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ppp_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PPP_DLFILE)
endif
