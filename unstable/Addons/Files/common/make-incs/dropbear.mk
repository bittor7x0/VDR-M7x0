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
DROPBEAR_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

ifeq ($(CONFIG_DROPBEAR),y)
ifneq ($(CONFIG_ZLIB),y)
   $(error dependency error: dropbear needs zlib enabled)
endif
endif

DROPBEAR_VERSION := 0.52
DROPBEAR_PATCHES_DIR := $(PATCHES_DIR)/dropbear/$(DROPBEAR_VERSION)

#http://matt.ucc.asn.au/dropbear/releases/dropbear-0.52.tar.bz2
DROPBEAR_FILE := dropbear-$(DROPBEAR_VERSION).tar.bz2
DROPBEAR_DLFILE := $(DOWNLOAD_DIR)/$(DROPBEAR_FILE)
DROPBEAR_URL := http://matt.ucc.asn.au/dropbear/releases/$(DROPBEAR_FILE)
DROPBEAR_DIR := $(BUILD_DIR)/dropbear-$(DROPBEAR_VERSION)

DROPBEAR_INSTALLED = $(STAGEFILES_DIR)/.dropbear_installed

PACKS_RULES_$(CONFIG_DROPBEAR) += $(DROPBEAR_INSTALLED)
FILE_LISTS_$(CONFIG_DROPBEAR) += dropbear.lst

CLEAN_RULES += clean-dropbear
DISTCLEAN_RULES += distclean-dropbear

#
# download dropbear
#

$(DROPBEAR_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DROPBEAR_DLFILE) ] ; then \
	$(WGET) $(DROPBEAR_URL) -O $(DROPBEAR_DLFILE) ; \
	fi );
	$(TOUCH) $(DROPBEAR_DLFILE)

#
# unpack dropbear
#

$(STAGEFILES_DIR)/.dropbear_unpacked: $(DROPBEAR_DLFILE) \
                                           $(wildcard $(DROPBEAR_PATCHES_DIR)/*.patch) \
                                           $$(DROPBEAR_DEPS)
	-$(RM) -rf $(DROPBEAR_DIR)
	$(BZCAT) $(DROPBEAR_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_unpacked

#
# patch dropbear
#

$(STAGEFILES_DIR)/.dropbear_patched: $(STAGEFILES_DIR)/.dropbear_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(DROPBEAR_DIR) $(DROPBEAR_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_patched

#
# configure dropbear
#

$(STAGEFILES_DIR)/.dropbear_configured: $(STAGEFILES_DIR)/.dropbear_patched
	($(CD) $(DROPBEAR_DIR) ; $(UCLIBC_ENV) \
		$(DROPBEAR_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-lastlog \
			--disable-wtmp \
			--sysconfdir=/etc/dropbear)
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_configured

#
# compile dropbear
#

$(STAGEFILES_DIR)/.dropbear_compiled: $(STAGEFILES_DIR)/.dropbear_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(DROPBEAR_DIR) PROGRAMS="dropbear dbclient dropbearkey scp" MULTI=1
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_compiled

#
# install dropbear
#

$(STAGEFILES_DIR)/.dropbear_installed: $(STAGEFILES_DIR)/.dropbear_compiled
	$(CP) $(DROPBEAR_DIR)/dropbearmulti $(TARGET_ROOT)/usr/sbin/dropbearmulti
	chmod 755 $(TARGET_ROOT)/usr/sbin/dropbearmulti
	$(MKDIR) -p $(TARGET_ROOT)/etc/dropbear
	$(TOUCH) $(STAGEFILES_DIR)/.dropbear_installed


$(FILELIST_DIR)/dropbear.lst: $(STAGEFILES_DIR)/.dropbear_installed
	$(TOUCH) $(FILELIST_DIR)/dropbear.lst

.PHONY: clean-dropbear distclean-dropbear

clean-dropbear:
	-$(RM) -rf $(DROPBEAR_DIR)

#
# clean everthing else
#

distclean-dropbear:
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_configured
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dropbear_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DROPBEAR_DLFILE)
endif
