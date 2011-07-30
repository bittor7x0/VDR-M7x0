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
CLOOG_HOSTDEPS = $(PPL_HOSTINSTALLED) $(GMP_HOSTINSTALLED)

CLOOG_HOSTVERSION := 0.15.11
CLOOG_HOSTPATCHES_DIR := $(PATCHES_DIR)/cloog/$(CLOOG_HOSTVERSION)

CLOOG_HOSTFILE := cloog-ppl-$(CLOOG_HOSTVERSION).tar.gz
CLOOG_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CLOOG_HOSTFILE)
CLOOG_HOSTURL := ftp://gcc.gnu.org/pub/gcc/infrastructure/$(CLOOG_HOSTFILE)
CLOOG_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/cloog-ppl-$(CLOOG_HOSTVERSION)

CLOOG_HOSTINSTALLED = $(STAGEFILES_DIR)/.cloog_host_installed

PACKS_RULES_$(CONFIG_CLOOG) += $(CLOOG_HOSTINSTALLED)

CLEAN_RULES += clean-cloog
DISTCLEAN_RULES += distclean-cloog

#
# download cloog
#

$(CLOOG_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CLOOG_HOSTDLFILE) ] ; then \
	$(WGET) $(CLOOG_HOSTURL) -O $(CLOOG_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CLOOG_HOSTDLFILE)

#
# unpack cloog
#

$(STAGEFILES_DIR)/.cloog_host_unpacked: $(CLOOG_HOSTDLFILE) \
                                           $(wildcard $(CLOOG_HOSTPATCHES_DIR)/*.patch) \
                                           $$(CLOOG_HOSTDEPS)
	-$(RM) -rf $(CLOOG_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(CLOOG_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_unpacked

#
# patch cloog
#

$(STAGEFILES_DIR)/.cloog_host_patched: $(STAGEFILES_DIR)/.cloog_host_unpacked
	$(call patch_package, $(CLOOG_HOSTDIR), $(CLOOG_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_patched

#
# configure cloog
#

$(STAGEFILES_DIR)/.cloog_host_configured: $(STAGEFILES_DIR)/.cloog_host_patched
	($(CD) $(CLOOG_HOSTDIR) ; $(CLOOG_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--disable-shared \
			--with-host-libstdcxx='-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
			--with-ppl=$(PREFIX) \
			--with-gmp=$(PREFIX))
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_configured

#
# compile cloog
#

$(STAGEFILES_DIR)/.cloog_host_compiled: $(STAGEFILES_DIR)/.cloog_host_configured
	$(MAKE) -C $(CLOOG_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_compiled

#
# install cloog
#

$(STAGEFILES_DIR)/.cloog_host_installed: $(STAGEFILES_DIR)/.cloog_host_compiled
	$(MAKE) -C $(CLOOG_HOSTDIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.cloog_host_installed


.PHONY: clean-cloog distclean-cloog

clean-cloog:
	-$(RM) -rf $(CLOOG_HOSTDIR)

#
# clean everthing else
#

distclean-cloog:
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.cloog_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CLOOG_HOSTDLFILE)
endif
