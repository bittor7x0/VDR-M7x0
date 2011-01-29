# Copyright (C) 2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#

RSAENCODE_BIN := $(HOSTUTILS_PREFIX_BIN)/rsaencode
RSADECODE_BIN := $(HOSTUTILS_PREFIX_BIN)/rsadecode

RSACODER_HOSTFILE := rsacoder.tar.bz2
RSACODER_HOSTDLFILE := $(DOWNLOAD_DIR)/$(RSACODER_HOSTFILE)

RSACODER_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/rsacoder

CLEAN_RULES += clean-rsacoder-host
DISTCLEAN_RULES += distclean-rsacoder-host

#
# we cannot download the rsacoders there are no real download urls available
#

#
# unpack rsacoder
#

$(STAGEFILES_DIR)/.rsacoder_host_unpacked: $(RSACODER_HOSTDLFILE) $(TC_INIT_RULE)
	-$(RM) -rf $(RSACODER_HOSTDIR)
	$(BZCAT) $(RSACODER_HOSTDLFILE) | $(TAR) -C $(HOSTUTILS_BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.rsacoder_host_unpacked


#
# compile rsacoder
#

$(STAGEFILES_DIR)/.rsacoder_host_compiled: $(STAGEFILES_DIR)/.rsacoder_host_unpacked
	$(MAKE) -C $(RSACODER_HOSTDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.rsacoder_host_compiled

#
# install rsacoder
#

$(STAGEFILES_DIR)/.rsacoder_host_installed: $(STAGEFILES_DIR)/.rsacoder_host_compiled
	$(MAKE) -C $(RSACODER_HOSTDIR) SBINDIR=$(HOSTUTILS_PREFIX_BIN) install
	$(TOUCH) $(STAGEFILES_DIR)/.rsacoder_host_installed


$(RSAENCODE_BIN): $(STAGEFILES_DIR)/.rsacoder_host_installed
	$(TOUCH) $(RSAENCODE_BIN)

$(RSADECODE_BIN): $(STAGEFILES_DIR)/.rsacoder_host_installed
	$(TOUCH) $(RSADECODE_BIN)

.PHONY: clean-rsacoder-host distclean-rsacoder-host

clean-rsacoder-host:
	-$(RM) -rf $(RSACODER_HOSTDIR)

distclean-rsacoder-host:
	-$(RM) -f $(STAGEFILES_DIR)/.rsacoder_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.rsacoder_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.rsacoder_host_installed
