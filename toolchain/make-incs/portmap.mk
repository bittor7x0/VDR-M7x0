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

ifeq ($(CONFIG_PORTMAP),y)
ifneq ($(CONFIG_TCP_WRAPPERS),y)
   $(error dependency error: portmap needs tcp_wrappers enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
PORTMAP_DEPS = $(BASE_BUILD_STAGEFILE) $(TCP_WRAPPERS_INSTALLED)

PORTMAP_VERSION := 6.0
PORTMAP_PATCHES_DIR := $(PATCHES_DIR)/portmap/$(PORTMAP_VERSION)

PORTMAP_FILE := portmap-$(PORTMAP_VERSION).tgz
PORTMAP_DLFILE := $(DOWNLOAD_DIR)/$(PORTMAP_FILE)
PORTMAP_URL := http://neil.brown.name/portmap/$(PORTMAP_FILE)
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
	$(UCLIBC_ENV) $(MAKE) \
		-C $(PORTMAP_DIR) \
		CFLAGS="$(UCLIBC_CFLAGS) -I$(TARGET_ROOT)/usr/include -I$(TARGET_ROOT)/include -DHOSTS_ACCESS -DFACILITY=LOG_DAEMON -DIGNORE_SIGCHLD" \
		RPCUSER="nobody" \
		WRAP_LIB="-L$(TARGET_ROOT)/usr/lib -L$(TARGET_ROOT)/lib -lwrap" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.portmap_compiled

#
# install portmap
#

$(STAGEFILES_DIR)/.portmap_installed: $(STAGEFILES_DIR)/.portmap_compiled
	$(CP) -f $(PORTMAP_DIR)/portmap $(TARGET_ROOT)/usr/sbin/portmap
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
