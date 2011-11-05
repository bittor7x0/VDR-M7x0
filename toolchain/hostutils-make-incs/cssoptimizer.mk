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

CSSOPTIMIZER_BIN := $(HOSTUTILS_PREFIX_BIN)/cssoptimizer

CSSOPTIMIZER_HOSTVERSION := 1.1
CSSOPTIMIZER_HOSTFILE := cssoptimizer-$(CSSOPTIMIZER_HOSTVERSION).tgz
CSSOPTIMIZER_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CSSOPTIMIZER_HOSTFILE)

CSSOPTIMIZER_HOSTPATCHES_DIR := $(PATCHES_DIR)/cssoptimizer/$(CSSOPTIMIZER_HOSTVERSION)
CSSOPTIMIZER_HOSTURL := http://mabblog.com/getfile.php?file=6
CSSOPTIMIZER_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/cssoptimizer-$(CSSOPTIMIZER_HOSTVERSION)

CLEAN_RULES += clean-cssoptimizer-host
DISTCLEAN_RULES += distclean-cssoptimizer-host

#
# download cssoptimizer
#

$(CSSOPTIMIZER_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CSSOPTIMIZER_HOSTDLFILE) ] ; then \
	$(WGET) $(CSSOPTIMIZER_HOSTURL) -O $(CSSOPTIMIZER_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CSSOPTIMIZER_HOSTDLFILE)

#
# unpack cssoptimizer
#

$(STAGEFILES_DIR)/.cssoptimizer_host_unpacked: $(CSSOPTIMIZER_HOSTDLFILE) \
                                         $(wildcard $(CSSOPTIMIZER_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(CSSOPTIMIZER_HOSTDIR)
	$(MKDIR) -p $(CSSOPTIMIZER_HOSTDIR)
	$(TAR) -C $(CSSOPTIMIZER_HOSTDIR) -zf $(CSSOPTIMIZER_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.cssoptimizer_host_unpacked

#
# patch cssoptimizer
#

$(STAGEFILES_DIR)/.cssoptimizer_host_patched: $(STAGEFILES_DIR)/.cssoptimizer_host_unpacked
	$(call patch_package, $(CSSOPTIMIZER_HOSTDIR), $(CSSOPTIMIZER_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.cssoptimizer_host_patched

#
# compile cssoptimizer
#

$(STAGEFILES_DIR)/.cssoptimizer_host_compiled: $(STAGEFILES_DIR)/.cssoptimizer_host_patched
	$(MAKE) -C $(CSSOPTIMIZER_HOSTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.cssoptimizer_host_compiled

#
# install cssoptimizer
#

$(HOSTUTILS_PREFIX_BIN)/cssoptimizer: $(STAGEFILES_DIR)/.cssoptimizer_host_compiled
	$(CP) $(CSSOPTIMIZER_HOSTDIR)/cssoptimizer $(HOSTUTILS_PREFIX_BIN)/cssoptimizer

.PHONY: clean-cssoptimizer-host distclean-cssoptimizer-host

clean-cssoptimizer-host:
	-$(RM) -rf $(CSSOPTIMIZER_HOSTDIR)

distclean-cssoptimizer-host:
	-$(RM) -f $(STAGEFILES_DIR)/.cssoptimizer_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.cssoptimizer_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.cssoptimizer_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/cssoptimizer
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CSSOPTIMIZER_HOSTDLFILE)
endif
