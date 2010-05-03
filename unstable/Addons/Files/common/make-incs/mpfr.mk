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
MPFR_DEPS = $(BASE_BUILD_STAGEFILE) $(GMP_INSTALLED)

MPFR_VERSION := 2.4.2
MPFR_PATCHES_DIR := $(PATCHES_DIR)/mpfr/$(MPFR_VERSION)

MPFR_FILE := mpfr-$(MPFR_VERSION).tar.bz2
MPFR_DLFILE := $(DOWNLOAD_DIR)/$(MPFR_FILE)
MPFR_URL := http://ftp.gnu.org/gnu/mpfr/$(MPFR_FILE)
MPFR_DIR := $(BUILD_DIR)/mpfr-$(MPFR_VERSION)

MPFR_INSTALLED = $(STAGEFILES_DIR)/.mpfr_installed

PACKS_RULES_$(CONFIG_MPFR) += $(MPFR_INSTALLED)

CLEAN_RULES += clean-mpfr
DISTCLEAN_RULES += distclean-mpfr

#
# download mpfr
#

$(MPFR_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MPFR_DLFILE) ] ; then \
	$(WGET) $(MPFR_URL) -O $(MPFR_DLFILE) ; \
	fi );
	$(TOUCH) $(MPFR_DLFILE)

#
# unpack mpfr
#

$(STAGEFILES_DIR)/.mpfr_unpacked: $(MPFR_DLFILE) \
                                           $(wildcard $(MPFR_PATCHES_DIR)/*.patch) \
                                           $$(MPFR_DEPS)
	-$(RM) -rf $(MPFR_DIR)
	$(BZCAT) $(MPFR_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.mpfr_unpacked

#
# patch mpfr
#

$(STAGEFILES_DIR)/.mpfr_patched: $(STAGEFILES_DIR)/.mpfr_unpacked
	$(call patch_package, $(MPFR_DIR), $(MPFR_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mpfr_patched

#
# configure mpfr
#

$(STAGEFILES_DIR)/.mpfr_configured: $(STAGEFILES_DIR)/.mpfr_patched
	($(CD) $(MPFR_DIR) ; $(MPFR_DIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-gmp=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.mpfr_configured

#
# compile mpfr
#

$(STAGEFILES_DIR)/.mpfr_compiled: $(STAGEFILES_DIR)/.mpfr_configured
	$(UCLIBC_ENV) $(MAKE) -C $(MPFR_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.mpfr_compiled

#
# install mpfr
#

$(STAGEFILES_DIR)/.mpfr_installed: $(STAGEFILES_DIR)/.mpfr_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(MPFR_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.mpfr_installed


.PHONY: clean-mpfr distclean-mpfr

clean-mpfr:
	-$(RM) -rf $(MPFR_DIR)

#
# clean everthing else
#

distclean-mpfr:
	-$(RM) -f $(STAGEFILES_DIR)/.mpfr_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mpfr_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mpfr_configured
	-$(RM) -f $(STAGEFILES_DIR)/.mpfr_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mpfr_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MPFR_DLFILE)
endif
