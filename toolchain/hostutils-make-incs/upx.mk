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


UPX_BIN := $(HOSTUTILS_PREFIX_BIN)/upx


UPX_HOSTVERSION := 3.07
UPX_HOSTFILE := upx-$(UPX_HOSTVERSION)-src.tar.bz2
UPX_HOSTDLFILE := $(DOWNLOAD_DIR)/$(UPX_HOSTFILE)

UPX_HOSTPATCHES_DIR := $(PATCHES_DIR)/upx/$(UPX_HOSTVERSION)
UPX_HOSTURL := http://upx.sourceforge.net/download/$(UPX_HOSTFILE)
UPX_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/upx-$(UPX_HOSTVERSION)-src

CLEAN_RULES += clean-upx-host
DISTCLEAN_RULES += distclean-upx-host

#
# download upx
#

$(UPX_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UPX_HOSTDLFILE) ] ; then \
	$(WGET) $(UPX_HOSTURL) -O $(UPX_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(UPX_HOSTDLFILE)

#
# unpack upx
#

$(STAGEFILES_DIR)/.upx_host_unpacked: $(UPX_HOSTDLFILE) \
                                         $(wildcard $(UPX_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(UPX_HOSTDIR)
	$(BZCAT) $(UPX_HOSTDLFILE) | $(TAR) -C $(HOSTUTILS_BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.upx_host_unpacked

#
# patch upx
#

$(STAGEFILES_DIR)/.upx_host_patched: $(STAGEFILES_DIR)/.upx_host_unpacked
	$(call patch_package, $(UPX_HOSTDIR), $(UPX_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.upx_host_patched

#
# compile upx
#

$(STAGEFILES_DIR)/.upx_host_compiled: $(STAGEFILES_DIR)/.upx_host_patched $(UCL_LIB)
	$(MAKE) -C $(UPX_HOSTDIR) \
		CXXFLAGS="$(CXXFLAGS) -Wl,-rpath,$(PREFIX)/lib" \
		UPX_UCLDIR="$(UCL_HOSTDIR)" \
		DEFS="-DUCL_NO_ASM" \
		exeext="" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.upx_host_compiled

#
# install upx
#

$(HOSTUTILS_PREFIX_BIN)/upx: $(STAGEFILES_DIR)/.upx_host_compiled
	$(CP) $(UPX_HOSTDIR)/src/upx $(HOSTUTILS_PREFIX_BIN)/upx

.PHONY: clean-upx-host distclean-upx-host

clean-upx-host:
	-$(RM) -rf $(UPX_HOSTDIR)

distclean-upx-host:
	-$(RM) -f $(STAGEFILES_DIR)/.upx_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.upx_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.upx_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/upx
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UPX_HOSTDLFILE)
endif