# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2024 VDR-NG-EM Project
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

ifeq ($(and $(filter y,$(CONFIG_PORTMAP)),$(filter y,$(CONFIG_RPCBIND))),y)
   $(error conflict error: portmap and rpcbind can't be enabled simultaneously)
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
PORTMAP_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_LIBTIRPC),y)
	PORTMAP_DEPS +=  $(LIBTIRPC_INSTALLED)
endif
ifeq ($(CONFIG_TCP_WRAPPERS),y)
	PORTMAP_DEPS +=  $(TCP_WRAPPERS_INSTALLED)
endif

PORTMAP_VERSION := 6.0
PORTMAP_PATCHES_DIR := $(PATCHES_DIR)/portmap/$(PORTMAP_VERSION)

PORTMAP_FILE := portmap-$(PORTMAP_VERSION).tgz
PORTMAP_DLFILE := $(DOWNLOAD_DIR)/$(PORTMAP_FILE)
PORTMAP_URL := http://fossies.org/linux/misc/old/$(PORTMAP_FILE)
PORTMAP_DIR := $(BUILD_DIR)/portmap_$(PORTMAP_VERSION)

PORTMAP_INSTALLED = $(STAGEFILES_DIR)/.portmap_installed

PACKS_RULES_$(CONFIG_PORTMAP) += $(PORTMAP_INSTALLED)
FILE_LISTS_$(CONFIG_PORTMAP) += portmap.lst

CLEAN_RULES += clean-portmap
DISTCLEAN_RULES += distclean-portmap

#
# download portmap
#

$(PORTMAP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PORTMAP_DLFILE) ] ; then \
	$(WGET) $(PORTMAP_URL) -O $(PORTMAP_DLFILE) ; \
	fi );
	$(TOUCH) $(PORTMAP_DLFILE)

#
# unpack portmap
#

$(STAGEFILES_DIR)/.portmap_unpacked: $(PORTMAP_DLFILE) \
                                           $(wildcard $(PORTMAP_PATCHES_DIR)/*.patch) \
                                           $$(PORTMAP_DEPS)
	-$(RM) -rf $(PORTMAP_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(PORTMAP_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.portmap_unpacked

#
# patch portmap
#

$(STAGEFILES_DIR)/.portmap_patched: $(STAGEFILES_DIR)/.portmap_unpacked
	$(call patch_package, $(PORTMAP_DIR), $(PORTMAP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.portmap_patched

#
# compile portmap
#

$(STAGEFILES_DIR)/.portmap_compiled: $(STAGEFILES_DIR)/.portmap_patched
	$(UCLIBC_ENV_LTO_GC) \
	$(MAKE) \
		-C $(PORTMAP_DIR) \
		CFLAGS="$(UCLIBC_CFLAGS_LTO_GC) $(if $(CONFIG_LIBTIRPC),-I${TARGET_ROOT}/usr/include/tirpc) -I$(TARGET_ROOT)/usr/include -I$(TARGET_ROOT)/include" \
		RPCUSER="nobody" \
		NO_PIE=1 \
		LDLIBS="-L$(TARGET_ROOT)/usr/lib -L$(TARGET_ROOT)/lib$(if $(CONFIG_LIBTIRPC), -ltirpc)$(if $(CONFIG_TCP_WRAPPERS), -lwrap)" \
		$(if $(CONFIG_TCP_WRAPPERS),,NO_TCP_WRAPPER=1) \
		portmap
	$(TOUCH) $(STAGEFILES_DIR)/.portmap_compiled

#
# install portmap
#

$(STAGEFILES_DIR)/.portmap_installed: $(STAGEFILES_DIR)/.portmap_compiled
	$(CP) -f $(PORTMAP_DIR)/portmap $(TARGET_ROOT)/usr/sbin/portmap
	(if [ X"`$(GREP) portmap $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's,/usr/sbin/rpcbind -f -w,/usr/sbin/portmap,g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) portmap $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.halt`" = X"" ] ; then \
		$(SED) -i 's,/usr/bin/killall -15 rpcbind,/usr/bin/killall -15 portmap,g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.halt ; \
	fi);
	$(TOUCH) $(STAGEFILES_DIR)/.portmap_installed


$(FILELIST_DIR)/portmap.lst: $(STAGEFILES_DIR)/.portmap_installed
	$(TOUCH) $(FILELIST_DIR)/portmap.lst

.PHONY: clean-portmap distclean-portmap

clean-portmap:
	-$(RM) -rf $(PORTMAP_DIR)

#
# clean everthing else
#

distclean-portmap:
	-$(RM) -f $(STAGEFILES_DIR)/.portmap_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.portmap_patched
	-$(RM) -f $(STAGEFILES_DIR)/.portmap_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.portmap_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PORTMAP_DLFILE)
endif
