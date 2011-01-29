# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
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

LINUX_HEADERS_FILE := m7x0-linux-headers.tar.bz2
LINUX_HEADERS_DLFILE := $(DOWNLOAD_DIR)/$(LINUX_HEADERS_FILE)
LINUX_HEADERS_DIR := $(BUILD_DIR)/m7x0-linux-headers
LINUX_HEADERS_INSTALL_DIR = $(TARGET_ROOT)/usr/include

LINUX_HEADERS_INSTALLED = $(STAGEFILES_DIR)/.linux_headers_installed

BASE_RULES_y += $(STAGEFILES_DIR)/.linux_headers_installed
CLEAN_RULES += clean-linux-headers
DISTCLEAN_RULES += distclean-linux-headers

#
# download linux-headers
#

$(LINUX_HEADERS_DLFILE):
	$(SECHO) Cannot download m7x0-linux-headers yet!
	$(SECHO) Download them from Siemens-GPL website or from open7x0.org

#
# unpack linux-headers
#

$(STAGEFILES_DIR)/.linux_headers_unpacked: $(LINUX_HEADERS_DLFILE) $(TC_INIT_RULE)
	-$(RM) -rf $(LINUX_HEADERS_DIR)
	$(BZCAT) $(LINUX_HEADERS_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.linux_headers_unpacked

$(STAGEFILES_DIR)/.linux_headers_installed: $(STAGEFILES_DIR)/.linux_headers_unpacked
	$(MKDIR) -p $(LINUX_HEADERS_INSTALL_DIR)
	$(SVN) co http://svn.assembla.com/svn/VDR-M7x0/trunk/vdr-m7x0/m7x0_dvb $(LINUX_HEADERS_DIR)/include/linux/dvb
	$(CP) -RPp $(LINUX_HEADERS_DIR)/include/* \
		$(LINUX_HEADERS_INSTALL_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.linux_headers_installed

.PHONY: clean-linux-headers distclean-linux-headers

clean-linux-headers:
	-$(RM) -rf $(LINUX_HEADERS_DIR)

distclean-linux-headers:
	-$(RM) -f $(STAGEFILES_DIR)/.linux_headers_unpacked
