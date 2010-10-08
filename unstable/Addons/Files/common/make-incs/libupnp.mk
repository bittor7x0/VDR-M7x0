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
LIBUPNP_DEPS = $(BASE_BUILD_STAGEFILE)

LIBUPNP_VERSION := 1.6.6
LIBUPNP_PATCHES_DIR := $(PATCHES_DIR)/libupnp/$(LIBUPNP_VERSION)

LIBUPNP_FILE := libupnp-$(LIBUPNP_VERSION).tar.bz2
LIBUPNP_DLFILE := $(DOWNLOAD_DIR)/$(LIBUPNP_FILE)
LIBUPNP_URL := http://sourceforge.net/projects/pupnp/files/pupnp/LibUPnP%20$(LIBUPNP_VERSION)/$(LIBUPNP_FILE)/download
LIBUPNP_DIR := $(BUILD_DIR)/libupnp-$(LIBUPNP_VERSION)

LIBUPNP_INSTALLED = $(STAGEFILES_DIR)/.libupnp_installed

PACKS_RULES_$(CONFIG_LIBUPNP) += $(LIBUPNP_INSTALLED)
FILE_LISTS_$(CONFIG_LIBUPNP) += libupnp.lst

CLEAN_RULES += clean-libupnp
DISTCLEAN_RULES += distclean-libupnp

#
# download libupnp
#

$(LIBUPNP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBUPNP_DLFILE) ] ; then \
	$(WGET) $(LIBUPNP_URL) -O $(LIBUPNP_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBUPNP_DLFILE)

#
# unpack libupnp
#

$(STAGEFILES_DIR)/.libupnp_unpacked: $(LIBUPNP_DLFILE) \
                                           $(wildcard $(LIBUPNP_PATCHES_DIR)/*.patch) \
                                           $$(LIBUPNP_DEPS)
	-$(RM) -rf $(LIBUPNP_DIR)
	$(BZCAT) $(LIBUPNP_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.libupnp_unpacked

#
# patch libupnp
#

$(STAGEFILES_DIR)/.libupnp_patched: $(STAGEFILES_DIR)/.libupnp_unpacked
	$(call patch_package, $(LIBUPNP_DIR), $(LIBUPNP_PATCHES_DIR)/common)
ifeq ($(CONFIG_LIBUPNP_DLNA),y)
	$(call patch_package, $(LIBUPNP_DIR), $(LIBUPNP_PATCHES_DIR)/dlna)
endif

	$(TOUCH) $(STAGEFILES_DIR)/.libupnp_patched

#
# configure libupnp
#

$(STAGEFILES_DIR)/.libupnp_configured: $(STAGEFILES_DIR)/.libupnp_patched
	($(CD) $(LIBUPNP_DIR) ; $(UCLIBC_ENV) \
		$(LIBUPNP_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.libupnp_configured

#
# compile libupnp
#

$(STAGEFILES_DIR)/.libupnp_compiled: $(STAGEFILES_DIR)/.libupnp_configured
	$(UCLIBC_ENV) $(MAKE) -C $(LIBUPNP_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.libupnp_compiled

#
# install libupnp
#

$(STAGEFILES_DIR)/.libupnp_installed: $(STAGEFILES_DIR)/.libupnp_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(LIBUPNP_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.libupnp_installed


$(FILELIST_DIR)/libupnp.lst: $(STAGEFILES_DIR)/.libupnp_installed
	$(TOUCH) $(FILELIST_DIR)/libupnp.lst

.PHONY: clean-libupnp distclean-libupnp

clean-libupnp:
	-$(RM) -rf $(LIBUPNP_DIR)

#
# clean everthing else
#

distclean-libupnp:
	-$(RM) -f $(STAGEFILES_DIR)/.libupnp_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libupnp_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libupnp_configured
	-$(RM) -f $(STAGEFILES_DIR)/.libupnp_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libupnp_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBUPNP_DLFILE)
endif
