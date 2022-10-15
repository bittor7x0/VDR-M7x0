# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2021-2022 VDR-NG-EM Project
#
# More information can be found in the files COPYING and README.
#
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

ifeq ($(CONFIG_RPCBIND),y)
ifeq ($(CONFIG_PORTMAP),y)
   $(error conflict error: rpcbind and portmap can't be enabled simultaneously)
endif
ifneq ($(CONFIG_LIBTIRPC),y)
   $(error dependency error: rpcbind needs libtirpc enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
RPCBIND_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBTIRPC_INSTALLED)

ifeq ($(CONFIG_TCP_WRAPPERS),y)
	RPCBIND_DEPS +=  $(TCP_WRAPPERS_INSTALLED)
endif

RPCBIND_VERSION := 1.2.5
RPCBIND_PATCHES_DIR := $(PATCHES_DIR)/rpcbind/$(RPCBIND_VERSION)

RPCBIND_FILE := rpcbind-$(RPCBIND_VERSION).tar.bz2
RPCBIND_DLFILE := $(DOWNLOAD_DIR)/$(RPCBIND_FILE)
RPCBIND_URL := http://downloads.sourceforge.net/rpcbind/$(RPCBIND_FILE)
RPCBIND_DIR := $(BUILD_DIR)/rpcbind-$(RPCBIND_VERSION)

RPCBIND_INSTALLED = $(STAGEFILES_DIR)/.rpcbind_installed

PACKS_RULES_$(CONFIG_RPCBIND) += $(RPCBIND_INSTALLED)
FILE_LISTS_$(CONFIG_RPCBIND) += rpcbind.lst

CLEAN_RULES += clean-rpcbind
DISTCLEAN_RULES += distclean-rpcbind

#
# download rpcbind
#

$(RPCBIND_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(RPCBIND_DLFILE) ] ; then \
	$(WGET) $(RPCBIND_URL) -O $(RPCBIND_DLFILE) ; \
	fi );
	$(TOUCH) $(RPCBIND_DLFILE)

#
# unpack rpcbind
#

$(STAGEFILES_DIR)/.rpcbind_unpacked: $(RPCBIND_DLFILE) \
                                           $(wildcard $(RPCBIND_PATCHES_DIR)/*.patch) \
                                           $$(RPCBIND_DEPS)
	-$(RM) -rf $(RPCBIND_DIR)
	$(BZCAT) $(RPCBIND_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.rpcbind_unpacked

#
# patch rpcbind
#

$(STAGEFILES_DIR)/.rpcbind_patched: $(STAGEFILES_DIR)/.rpcbind_unpacked
	$(call patch_package, $(RPCBIND_DIR), $(RPCBIND_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.rpcbind_patched

#
# configure rpcbind
#			

$(STAGEFILES_DIR)/.rpcbind_configured: $(STAGEFILES_DIR)/.rpcbind_patched
	($(CD) $(RPCBIND_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		./configure \
			--prefix=$(TARGET_ROOT)/usr \
			--build=$(HOST_TARGET) \
			--host=$(TARGET) \
			--target=$(TARGET) \
			--with-rpcuser=nobody \
			--without-systemdsystemunitdir \
			--enable-warmstarts \
			$(if $(CONFIG_TCP_WRAPPERS),--enable-libwrap,--disable-libwrap) \
			$(if $(CONFIG_RPCBIND_RMTCALLS),--enable-rmtcalls,--disable-rmtcalls))
	$(TOUCH) $(STAGEFILES_DIR)/.rpcbind_configured

#
# compile rpcbind
#

$(STAGEFILES_DIR)/.rpcbind_compiled: $(STAGEFILES_DIR)/.rpcbind_configured
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(RPCBIND_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.rpcbind_compiled

#
# install rpcbind
#

$(STAGEFILES_DIR)/.rpcbind_installed: $(STAGEFILES_DIR)/.rpcbind_compiled
	$(CP) -f $(RPCBIND_DIR)/rpcinfo $(TARGET_ROOT)/usr/bin/rpcinfo
	$(CP) -f $(RPCBIND_DIR)/rpcbind $(TARGET_ROOT)/usr/sbin/rpcbind
	(if [ X"`$(GREP) rpcbind $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's,/usr/sbin/portmap,/usr/sbin/rpcbind -f -w,g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) rpcbind $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.halt`" = X"" ] ; then \
		$(SED) -i 's,/usr/bin/killall -15 portmap,/usr/bin/killall -15 rpcbind,g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.halt ; \
	fi);
	$(TOUCH) $(STAGEFILES_DIR)/.rpcbind_installed


$(FILELIST_DIR)/rpcbind.lst: $(STAGEFILES_DIR)/.rpcbind_installed
	$(TOUCH) $(FILELIST_DIR)/rpcbind.lst

.PHONY: clean-rpcbind distclean-rpcbind

clean-rpcbind:
	-$(RM) -rf $(RPCBIND_DIR)

#
# clean everthing else
#

distclean-rpcbind:
	-$(RM) -f $(STAGEFILES_DIR)/.rpcbind_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.rpcbind_patched
	-$(RM) -f $(STAGEFILES_DIR)/.rpcbind_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.rpcbind_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(RPCBIND_DLFILE)
endif
