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


UCL_LIB := $(PREFIX)/lib/libucl.so


UCL_HOSTVERSION := 1.03
UCL_HOSTFILE := ucl-$(UCL_HOSTVERSION).tar.gz
UCL_HOSTDLFILE := $(DOWNLOAD_DIR)/$(UCL_HOSTFILE)

UCL_HOSTPATCHES_DIR := $(PATCHES_DIR)/ucl/$(UCL_HOSTVERSION)
UCL_HOSTURL := http://www.oberhumer.com/opensource/ucl/download/$(UCL_HOSTFILE)
UCL_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/ucl-$(UCL_HOSTVERSION)

CLEAN_RULES += clean-ucl-host
DISTCLEAN_RULES += distclean-ucl-host

#
# download ucl
#

$(UCL_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UCL_HOSTDLFILE) ] ; then \
	$(WGET) $(UCL_HOSTURL) -O $(UCL_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(UCL_HOSTDLFILE)

#
# unpack ucl
#

$(STAGEFILES_DIR)/.ucl_host_unpacked: $(UCL_HOSTDLFILE) \
                                         $(wildcard $(UCL_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(UCL_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(UCL_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.ucl_host_unpacked

#
# patch ucl
#

$(STAGEFILES_DIR)/.ucl_host_patched: $(STAGEFILES_DIR)/.ucl_host_unpacked
	$(call patch_package, $(UCL_HOSTDIR), $(UCL_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ucl_host_patched

#
# configure ucl
#

$(STAGEFILES_DIR)/.ucl_host_configured: $(STAGEFILES_DIR)/.ucl_host_patched
	-$(MAKE) -C $(UCL_HOSTDIR) distclean
	($(CD) $(UCL_HOSTDIR) ; \
		$(UCL_HOSTDIR)/configure \
			--prefix=$(PREFIX) \
			--enable-static \
			--enable-shared \
			--disable-asm)
	$(TOUCH) $(STAGEFILES_DIR)/.ucl_host_configured

#
# compile ucl
#

$(STAGEFILES_DIR)/.ucl_host_compiled: $(STAGEFILES_DIR)/.ucl_host_configured
	$(MAKE) -C $(UCL_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ucl_host_compiled

#
# install ucl
#

$(PREFIX)/lib/libucl.so: $(STAGEFILES_DIR)/.ucl_host_compiled
	$(MAKE) -C $(UCL_HOSTDIR) install

.PHONY: clean-ucl-host distclean-ucl-host

clean-ucl-host:
	-$(RM) -rf $(UCL_HOSTDIR)

distclean-ucl-host:
	-$(RM) -f $(STAGEFILES_DIR)/.ucl_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ucl_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ucl_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ucl_host_compiled
	-$(RM) -f $(PREFIX)/lib/libucl.so
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UCL_HOSTDLFILE)
endif