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

ifeq ($(CONFIG_USHARE),y)
ifneq ($(CONFIG_LIBUPNP),y)
   $(error dependency error: ushare needs libupnp enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
USHARE_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBUPNP_INSTALLED)

ifeq ($(CONFIG_LIBDLNA),y)
	USHARE_DEPS +=  $(LIBDLNA_INSTALLED)
endif

USHARE_VERSION := 1.1a
USHARE_PATCHES_DIR := $(PATCHES_DIR)/ushare/$(USHARE_VERSION)

USHARE_FILE := ushare-$(USHARE_VERSION).tar.bz2
USHARE_DLFILE := $(DOWNLOAD_DIR)/$(USHARE_FILE)
USHARE_URL := http://ushare.geexbox.org/releases/$(USHARE_FILE)
USHARE_DIR := $(BUILD_DIR)/ushare-$(USHARE_VERSION)

USHARE_INSTALLED = $(STAGEFILES_DIR)/.ushare_installed

PACKS_RULES_$(CONFIG_USHARE) += $(USHARE_INSTALLED)
FILE_LISTS_$(CONFIG_USHARE) += ushare.lst

CLEAN_RULES += clean-ushare
DISTCLEAN_RULES += distclean-ushare

#
# download ushare
#

$(USHARE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(USHARE_DLFILE) ] ; then \
	$(WGET) $(USHARE_URL) -O $(USHARE_DLFILE) ; \
	fi );
	$(TOUCH) $(USHARE_DLFILE)

#
# unpack ushare
#

$(STAGEFILES_DIR)/.ushare_unpacked: $(USHARE_DLFILE) \
                                           $(wildcard $(USHARE_PATCHES_DIR)/*.patch) \
                                           $$(USHARE_DEPS)
	-$(RM) -rf $(USHARE_DIR)
	$(BZCAT) $(USHARE_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.ushare_unpacked

#
# patch ushare
#

$(STAGEFILES_DIR)/.ushare_patched: $(STAGEFILES_DIR)/.ushare_unpacked
	$(call patch_package, $(USHARE_DIR), $(USHARE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ushare_patched

#
# configure ushare (this is *NOT* GNU configure)
# 

$(STAGEFILES_DIR)/.ushare_configured: $(STAGEFILES_DIR)/.ushare_patched
	($(CD) $(USHARE_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="-L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(USHARE_DIR)/configure \
		--cross-compile \
		--cross-prefix=$(UCLIBC_TARGET)- \
		--prefix=/usr \
		--sysconfdir=/etc \
		$(if $(CONFIG_LIBDLNA),--enable-dlna) \
		--disable-nls \
		--disable-debug \
		--disable-optimize \
		--disable-strip)
	$(TOUCH) $(STAGEFILES_DIR)/.ushare_configured

#
# compile ushare
#

$(STAGEFILES_DIR)/.ushare_compiled: $(STAGEFILES_DIR)/.ushare_configured
	$(UCLIBC_ENV) $(MAKE) -C $(USHARE_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.ushare_compiled

#
# install ushare
#

$(STAGEFILES_DIR)/.ushare_installed: $(STAGEFILES_DIR)/.ushare_compiled
	$(CP) -f $(USHARE_DIR)/src/ushare $(TARGET_ROOT)/usr/bin/ushare
	$(CP) -f $(USHARE_DIR)/scripts/ushare.conf $(TARGET_ROOT)/etc/ushare.conf
	$(TOUCH) $(STAGEFILES_DIR)/.ushare_installed


$(FILELIST_DIR)/ushare.lst: $(STAGEFILES_DIR)/.ushare_installed
	$(TOUCH) $(FILELIST_DIR)/ushare.lst

.PHONY: clean-ushare distclean-ushare

clean-ushare:
	-$(RM) -rf $(USHARE_DIR)

#
# clean everthing else
#

distclean-ushare:
	-$(RM) -f $(STAGEFILES_DIR)/.ushare_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ushare_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ushare_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ushare_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ushare_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(USHARE_DLFILE)
endif
