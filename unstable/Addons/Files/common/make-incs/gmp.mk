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
GMP_DEPS = $(BASE_BUILD_STAGEFILE)

GMP_VERSION := 5.0.1
GMP_PATCHES_DIR := $(PATCHES_DIR)/gmp/$(GMP_VERSION)

GMP_FILE := gmp-$(GMP_VERSION).tar.bz2
GMP_DLFILE := $(DOWNLOAD_DIR)/$(GMP_FILE)
GMP_URL := http://ftp.gnu.org/gnu/gmp/$(GMP_FILE)
GMP_DIR := $(BUILD_DIR)/gmp-$(GMP_VERSION)

GMP_INSTALLED = $(STAGEFILES_DIR)/.gmp_installed

PACKS_RULES_$(CONFIG_GMP) += $(GMP_INSTALLED)

CLEAN_RULES += clean-gmp
DISTCLEAN_RULES += distclean-gmp

#
# download gmp
#

$(GMP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GMP_DLFILE) ] ; then \
	$(WGET) $(GMP_URL) -O $(GMP_DLFILE) ; \
	fi );
	$(TOUCH) $(GMP_DLFILE)

#
# unpack gmp
#

$(STAGEFILES_DIR)/.gmp_unpacked: $(GMP_DLFILE) \
                                           $(wildcard $(GMP_PATCHES_DIR)/*.patch) \
                                           $$(GMP_DEPS)
	-$(RM) -rf $(GMP_DIR)
	$(BZCAT) $(GMP_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_unpacked

#
# patch gmp
#

$(STAGEFILES_DIR)/.gmp_patched: $(STAGEFILES_DIR)/.gmp_unpacked
	$(call patch_package, $(GMP_DIR), $(GMP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_patched

#
# configure gmp
#

$(STAGEFILES_DIR)/.gmp_configured: $(STAGEFILES_DIR)/.gmp_patched
	($(CD) $(GMP_DIR) ; $(GMP_DIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--enable-cxx \
			--enable-mpbsd)
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_configured

#
# compile gmp
#

$(STAGEFILES_DIR)/.gmp_compiled: $(STAGEFILES_DIR)/.gmp_configured
	$(UCLIBC_ENV) $(MAKE) -C $(GMP_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_compiled

#
# install gmp
#

$(STAGEFILES_DIR)/.gmp_installed: $(STAGEFILES_DIR)/.gmp_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(GMP_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.gmp_installed


.PHONY: clean-gmp distclean-gmp

clean-gmp:
	-$(RM) -rf $(GMP_DIR)

#
# clean everthing else
#

distclean-gmp:
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_patched
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gmp_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GMP_DLFILE)
endif
