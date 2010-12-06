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
LYNX_DEPS = $(BASE_BUILD_STAGEFILE) $(NCURSES_INSTALLED)

ifeq ($(CONFIG_ZLIB),y)
   LYNX_DEPS += $(ZLIB_INSTALLED)
endif

ifeq ($(CONFIG_LYNX),y)
ifneq ($(CONFIG_NCURSES),y)
   $(error dependency error: lynx needs ncurses enabled)
endif
endif

LYNX_VERSION := 2.8.7
LYNX_PATCHES_DIR := $(PATCHES_DIR)/lynx/$(LYNX_VERSION)

LYNX_FILE := lynx$(LYNX_VERSION).tar.bz2
LYNX_DLFILE := $(DOWNLOAD_DIR)/$(LYNX_FILE)
LYNX_URL := http://lynx.isc.org/lynx$(LYNX_VERSION)/$(LYNX_FILE)
LYNX_DIR := $(BUILD_DIR)/lynx-$(LYNX_VERSION)

LYNX_INSTALLED = $(STAGEFILES_DIR)/.lynx_installed

PACKS_RULES_$(CONFIG_LYNX) += $(LYNX_INSTALLED)
FILE_LISTS_$(CONFIG_LYNX) += lynx.lst

CLEAN_RULES += clean-lynx
DISTCLEAN_RULES += distclean-lynx

#
# download lynx
#

$(LYNX_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LYNX_DLFILE) ] ; then \
	$(WGET) $(LYNX_URL) -O $(LYNX_DLFILE) ; \
	fi );
	$(TOUCH) $(LYNX_DLFILE)

#
# unpack lynx
#

$(STAGEFILES_DIR)/.lynx_unpacked: $(LYNX_DLFILE) \
                                           $(wildcard $(LYNX_PATCHES_DIR)/*.patch) \
                                           $$(LYNX_DEPS)
	-$(RM) -rf $(LYNX_DIR)
	-$(RM) -rf $(BUILD_DIR)/lynx*
	$(BZCAT) $(LYNX_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(MV) $(BUILD_DIR)/lynx* $(LYNX_DIR) 
	$(TOUCH) $(STAGEFILES_DIR)/.lynx_unpacked

#
# patch lynx
#

$(STAGEFILES_DIR)/.lynx_patched: $(STAGEFILES_DIR)/.lynx_unpacked
	$(call patch_package, $(LYNX_DIR), $(LYNX_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.lynx_patched

#
# configure lynx
#

$(STAGEFILES_DIR)/.lynx_configured: $(STAGEFILES_DIR)/.lynx_patched
	($(CD) $(LYNX_DIR) ; $(UCLIBC_ENV) \
		$(LYNX_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--sysconfdir=/etc/lynx \
			--with-screen=ncurses \
			--with-curses-dir=$(TARGET_ROOT) \
			$(if $(CONFIG_ZLIB),--with-zlib=$(TARGET_ROOT),) \
			--with-charsets=iso-8859-15 \
			--enable-syslog \
			--enable-color-style \
			--enable-justify-elts \
			--enable-scrollbar \
			--disable-bibp-urls \
			--disable-dired \
			--disable-persistent-cookies \
			--disable-alt-bindings \
			--disable-config-info \
			--disable-finger \
			--disable-gopher \
			--disable-news \
			--disable-ftp \
			--disable-file-upload \
			--disable-full-paths)
	$(TOUCH) $(STAGEFILES_DIR)/.lynx_configured

#
# compile lynx
#

$(STAGEFILES_DIR)/.lynx_compiled: $(STAGEFILES_DIR)/.lynx_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LYNX_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.lynx_compiled

#
# install lynx
#

$(STAGEFILES_DIR)/.lynx_installed: $(STAGEFILES_DIR)/.lynx_compiled
	$(CP) $(LYNX_DIR)/lynx $(TARGET_ROOT)/usr/bin/lynx
	$(MKDIR) -p $(TARGET_ROOT)/etc/lynx
	$(CP) $(LYNX_DIR)/lynx.cfg $(TARGET_ROOT)/etc/lynx/lynx.cfg
	$(CP) $(LYNX_DIR)/samples/lynx.lss $(TARGET_ROOT)/etc/lynx/lynx.lss
	$(TOUCH) $(STAGEFILES_DIR)/.lynx_installed


$(FILELIST_DIR)/lynx.lst: $(STAGEFILES_DIR)/.lynx_installed
	$(TOUCH) $(FILELIST_DIR)/lynx.lst

.PHONY: clean-lynx distclean-lynx

clean-lynx:
	-$(RM) -rf $(LYNX_DIR)

#
# clean everthing else
#

distclean-lynx:
	-$(RM) -f $(STAGEFILES_DIR)/.lynx_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.lynx_patched
	-$(RM) -f $(STAGEFILES_DIR)/.lynx_configured
	-$(RM) -f $(STAGEFILES_DIR)/.lynx_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.lynx_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LYNX_DLFILE)
endif
