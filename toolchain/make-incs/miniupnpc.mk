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
MINIUPNPC_DEPS = $(BASE_BUILD_STAGEFILE)

MINIUPNPC_VERSION := 1.4
MINIUPNPC_PATCHES_DIR := $(PATCHES_DIR)/miniupnpc/$(MINIUPNPC_VERSION)

MINIUPNPC_FILE := miniupnpc-$(MINIUPNPC_VERSION).tar.gz
MINIUPNPC_DLFILE := $(DOWNLOAD_DIR)/$(MINIUPNPC_FILE)
MINIUPNPC_URL := http://miniupnp.free.fr/files/$(MINIUPNPC_FILE)
MINIUPNPC_DIR := $(BUILD_DIR)/miniupnpc-$(MINIUPNPC_VERSION)

MINIUPNPC_INSTALLED = $(STAGEFILES_DIR)/.miniupnpc_installed

PACKS_RULES_$(CONFIG_MINIUPNPC) += $(MINIUPNPC_INSTALLED)
FILE_LISTS_$(CONFIG_MINIUPNPC) += miniupnpc.lst

CLEAN_RULES += clean-miniupnpc
DISTCLEAN_RULES += distclean-miniupnpc

#
# download miniupnpc
#

$(MINIUPNPC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MINIUPNPC_DLFILE) ] ; then \
	$(WGET) $(MINIUPNPC_URL) -O $(MINIUPNPC_DLFILE) ; \
	fi );
	$(TOUCH) $(MINIUPNPC_DLFILE)

#
# unpack miniupnpc
#

$(STAGEFILES_DIR)/.miniupnpc_unpacked: $(MINIUPNPC_DLFILE) \
                                           $(wildcard $(MINIUPNPC_PATCHES_DIR)/*.patch) \
                                           $$(MINIUPNPC_DEPS)
	-$(RM) -rf $(MINIUPNPC_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(MINIUPNPC_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.miniupnpc_unpacked

#
# patch miniupnpc
#

$(STAGEFILES_DIR)/.miniupnpc_patched: $(STAGEFILES_DIR)/.miniupnpc_unpacked
	$(call patch_package, $(MINIUPNPC_DIR), $(MINIUPNPC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.miniupnpc_patched

#
# compile miniupnpc
#

$(STAGEFILES_DIR)/.miniupnpc_compiled: $(STAGEFILES_DIR)/.miniupnpc_patched
	$(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" \
		$(MAKE) -C $(MINIUPNPC_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.miniupnpc_compiled

#
# install miniupnpc
#

$(STAGEFILES_DIR)/.miniupnpc_installed: $(STAGEFILES_DIR)/.miniupnpc_compiled
	$(UCLIBC_ENV) CFLAGS="$(UCLIBC_CFLAGS) -fPIC" \
		INSTALLPREFIX=$(TARGET_ROOT)/usr \
		$(MAKE) -C $(MINIUPNPC_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.miniupnpc_installed


$(FILELIST_DIR)/miniupnpc.lst: $(STAGEFILES_DIR)/.miniupnpc_installed
	$(TOUCH) $(FILELIST_DIR)/miniupnpc.lst

.PHONY: clean-miniupnpc distclean-miniupnpc

clean-miniupnpc:
	-$(RM) -rf $(MINIUPNPC_DIR)

#
# clean everthing else
#

distclean-miniupnpc:
	-$(RM) -f $(STAGEFILES_DIR)/.miniupnpc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.miniupnpc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.miniupnpc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.miniupnpc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MINIUPNPC_DLFILE)
endif
