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
# The project's page is at http://www.open7x0.org
#

ifeq ($(CONFIG_GMEDIASERVER),y)
ifneq ($(and $(filter y,$(CONFIG_LIBUPNP)),$(filter y,$(CONFIG_LIBMAGIC))),y)
   $(error dependency error: gmediaserver needs libupnp and libmagic enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
GMEDIASERVER_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBUPNP_INSTALLED) $(LIBMAGIC_INSTALLED)

ifeq ($(CONFIG_TAGLIB),y)
	GMEDIASERVER_DEPS +=  $(TAGLIB_INSTALLED)
endif
ifeq ($(CONFIG_ID3LIB),y)
	GMEDIASERVER_DEPS +=  $(ID3LIB_INSTALLED)
endif
ifeq ($(CONFIG_LIBICONV),y)
	GMEDIASERVER_DEPS +=  $(LIBICONV_INSTALLED)
endif
ifeq ($(CONFIG_LIBUUID),y)
	GMEDIASERVER_DEPS +=  $(LIBUUID_INSTALLED)
endif

GMEDIASERVER_VERSION := 0.13.0
GMEDIASERVER_PATCHES_DIR := $(PATCHES_DIR)/gmediaserver/$(GMEDIASERVER_VERSION)

GMEDIASERVER_FILE := gmediaserver-$(GMEDIASERVER_VERSION).tar.gz
GMEDIASERVER_DLFILE := $(DOWNLOAD_DIR)/$(GMEDIASERVER_FILE)
GMEDIASERVER_URL := http://savannah.nongnu.org/download/gmediaserver/$(GMEDIASERVER_FILE)
GMEDIASERVER_DIR := $(BUILD_DIR)/gmediaserver-$(GMEDIASERVER_VERSION)

GMEDIASERVER_INSTALLED = $(STAGEFILES_DIR)/.gmediaserver_installed

PACKS_RULES_$(CONFIG_GMEDIASERVER) += $(GMEDIASERVER_INSTALLED)
FILE_LISTS_$(CONFIG_GMEDIASERVER) += gmediaserver.lst

CLEAN_RULES += clean-gmediaserver
DISTCLEAN_RULES += distclean-gmediaserver

#
# download gmediaserver
#

$(GMEDIASERVER_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GMEDIASERVER_DLFILE) ] ; then \
	$(WGET) $(GMEDIASERVER_URL) -O $(GMEDIASERVER_DLFILE) ; \
	fi );
	$(TOUCH) $(GMEDIASERVER_DLFILE)

#
# unpack gmediaserver
#

$(STAGEFILES_DIR)/.gmediaserver_unpacked: $(GMEDIASERVER_DLFILE) \
                                           $(wildcard $(GMEDIASERVER_PATCHES_DIR)/*.patch) \
                                           $$(GMEDIASERVER_DEPS)
	-$(RM) -rf $(GMEDIASERVER_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(GMEDIASERVER_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.gmediaserver_unpacked

#
# patch gmediaserver
#

$(STAGEFILES_DIR)/.gmediaserver_patched: $(STAGEFILES_DIR)/.gmediaserver_unpacked
	$(call patch_package, $(GMEDIASERVER_DIR), $(GMEDIASERVER_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.gmediaserver_patched

#
# configure gmediaserver
# 

$(STAGEFILES_DIR)/.gmediaserver_configured: $(STAGEFILES_DIR)/.gmediaserver_patched
	($(CD) $(GMEDIASERVER_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="-L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib" \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(GMEDIASERVER_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-rpath \
			$(if $(CONFIG_LIBICONV),--with-libiconv-prefix="$(TARGET_ROOT)/usr/lib/libiconv",--without-libiconv) \
			--without-libintl-prefix \
			$(if $(CONFIG_ID3LIB),--with-id3lib="$(TARGET_ROOT)/usr",--without-id3lib) \
			--with-libupnp="$(TARGET_ROOT)/usr")
	$(TOUCH) $(STAGEFILES_DIR)/.gmediaserver_configured

#
# compile gmediaserver
#

$(STAGEFILES_DIR)/.gmediaserver_compiled: $(STAGEFILES_DIR)/.gmediaserver_configured
	$(UCLIBC_ENV) $(MAKE) -C $(GMEDIASERVER_DIR) \
		AR="$(UCLIBC_AR)" \
		LTLIBICONV=""
	$(TOUCH) $(STAGEFILES_DIR)/.gmediaserver_compiled

#
# install gmediaserver
#

$(STAGEFILES_DIR)/.gmediaserver_installed: $(STAGEFILES_DIR)/.gmediaserver_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(GMEDIASERVER_DIR) \
		AR="$(UCLIBC_AR)" \
		LTLIBICONV="" \
		install
	$(TOUCH) $(STAGEFILES_DIR)/.gmediaserver_installed


$(FILELIST_DIR)/gmediaserver.lst: $(STAGEFILES_DIR)/.gmediaserver_installed
	$(TOUCH) $(FILELIST_DIR)/gmediaserver.lst

.PHONY: clean-gmediaserver distclean-gmediaserver

clean-gmediaserver:
	-$(RM) -rf $(GMEDIASERVER_DIR)

#
# clean everthing else
#

distclean-gmediaserver:
	-$(RM) -f $(STAGEFILES_DIR)/.gmediaserver_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.gmediaserver_patched
	-$(RM) -f $(STAGEFILES_DIR)/.gmediaserver_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gmediaserver_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gmediaserver_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GMEDIASERVER_DLFILE)
endif
