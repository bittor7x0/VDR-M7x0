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

# Put dependencies here
PPL_HOSTDEPS = $(GMP_HOSTINSTALLED)

PPL_HOSTVERSION := 0.11
PPL_HOSTPATCHES_DIR := $(PATCHES_DIR)/ppl/$(PPL_HOSTVERSION)

PPL_HOSTFILE := ppl-$(PPL_HOSTVERSION).tar.gz
PPL_HOSTDLFILE := $(DOWNLOAD_DIR)/$(PPL_HOSTFILE)
PPL_HOSTURL := ftp://gcc.gnu.org/pub/gcc/infrastructure/$(PPL_HOSTFILE)
PPL_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/ppl-$(PPL_HOSTVERSION)

PPL_HOSTINSTALLED = $(STAGEFILES_DIR)/.ppl_host_installed

PACKS_RULES_$(CONFIG_PPL) += $(PPL_HOSTINSTALLED)

CLEAN_RULES += clean-ppl
DISTCLEAN_RULES += distclean-ppl

#
# download ppl
#

$(PPL_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PPL_HOSTDLFILE) ] ; then \
	$(WGET) $(PPL_HOSTURL) -O $(PPL_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(PPL_HOSTDLFILE)

#
# unpack ppl
#

$(STAGEFILES_DIR)/.ppl_host_unpacked: $(PPL_HOSTDLFILE) \
                                           $(wildcard $(PPL_HOSTPATCHES_DIR)/*.patch) \
                                           $$(PPL_HOSTDEPS)
	-$(RM) -rf $(PPL_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(PPL_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.ppl_host_unpacked

#
# patch ppl
#

$(STAGEFILES_DIR)/.ppl_host_patched: $(STAGEFILES_DIR)/.ppl_host_unpacked
	$(call patch_package, $(PPL_HOSTDIR), $(PPL_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ppl_host_patched

#
# configure ppl
#

$(STAGEFILES_DIR)/.ppl_host_configured: $(STAGEFILES_DIR)/.ppl_host_patched
	($(CD) $(PPL_HOSTDIR) ; $(PPL_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--disable-pch \
			--disable-watchdog \
			--disable-ppl_lcdd \
			--disable-ppl_lpsol \
			--disable-ppl_pips \
			--with-gmp=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.ppl_host_configured

#
# compile ppl
#

$(STAGEFILES_DIR)/.ppl_host_compiled: $(STAGEFILES_DIR)/.ppl_host_configured
	$(MAKE) -C $(PPL_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ppl_host_compiled

#
# install ppl
#

$(STAGEFILES_DIR)/.ppl_host_installed: $(STAGEFILES_DIR)/.ppl_host_compiled
	$(MAKE) -C $(PPL_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.ppl_host_installed


.PHONY: clean-ppl distclean-ppl

clean-ppl:
	-$(RM) -rf $(PPL_HOSTDIR)

#
# clean everthing else
#

distclean-ppl:
	-$(RM) -f $(STAGEFILES_DIR)/.ppl_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ppl_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ppl_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ppl_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ppl_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PPL_HOSTDLFILE)
endif
