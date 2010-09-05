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

# Put dependencies here
MPC_DEPS = $(GMP_INSTALLED) $(MPFR_INSTALLED)

MPC_VERSION := 0.8.2
MPC_PATCHES_DIR := $(PATCHES_DIR)/mpc/$(MPC_VERSION)

MPC_FILE := mpc-$(MPC_VERSION).tar.gz
MPC_DLFILE := $(DOWNLOAD_DIR)/$(MPC_FILE)
MPC_URL := http://www.multiprecision.org/mpc/download/$(MPC_FILE)
MPC_DIR := $(BUILD_DIR)/mpc-$(MPC_VERSION)

MPC_INSTALLED = $(STAGEFILES_DIR)/.mpc_installed

PACKS_RULES_$(CONFIG_MPC) += $(MPC_INSTALLED)

CLEAN_RULES += clean-mpc
DISTCLEAN_RULES += distclean-mpc

#
# download mpc
#

$(MPC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MPC_DLFILE) ] ; then \
	$(WGET) $(MPC_URL) -O $(MPC_DLFILE) ; \
	fi );
	$(TOUCH) $(MPC_DLFILE)

#
# unpack mpc
#

$(STAGEFILES_DIR)/.mpc_unpacked: $(MPC_DLFILE) \
                                           $(wildcard $(MPC_PATCHES_DIR)/*.patch) \
                                           $$(MPC_DEPS)
	-$(RM) -rf $(MPC_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(MPC_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_unpacked

#
# patch mpc
#

$(STAGEFILES_DIR)/.mpc_patched: $(STAGEFILES_DIR)/.mpc_unpacked
	$(call patch_package, $(MPC_DIR), $(MPC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_patched

#
# configure mpc
#

$(STAGEFILES_DIR)/.mpc_configured: $(STAGEFILES_DIR)/.mpc_patched
	($(CD) $(MPC_DIR) ; $(MPC_DIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-mpfr=$(PREFIX) \
			--with-gmp=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_configured

#
# compile mpc
#

$(STAGEFILES_DIR)/.mpc_compiled: $(STAGEFILES_DIR)/.mpc_configured
	$(UCLIBC_ENV) $(MAKE) -C $(MPC_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_compiled

#
# install mpc
#

$(STAGEFILES_DIR)/.mpc_installed: $(STAGEFILES_DIR)/.mpc_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(MPC_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.mpc_installed


.PHONY: clean-mpc distclean-mpc

clean-mpc:
	-$(RM) -rf $(MPC_DIR)

#
# clean everthing else
#

distclean-mpc:
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mpc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MPC_DLFILE)
endif
