# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2024 VDR-NG-EM Project
#
# More information can be found in the files COPYING and README.
#
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

LINUX_HEADERS_FILE := m7x0-linux-headers.tar.bz2
LINUX_HEADERS_DLFILE := $(DOWNLOAD_DIR)/$(LINUX_HEADERS_FILE)
LINUX_HEADERS_DIR := $(BUILD_DIR)/m7x0-linux-headers
LINUX_HEADERS_GIT_SUBDIR := vdr-m7x0/m7x0_dvb
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
	$(call git_clone_subdir, $(GIT_VDR-M7x0_REPO_URL), /$(LINUX_HEADERS_GIT_SUBDIR), $(LINUX_HEADERS_DIR)/include/linux/dvb-git)
	$(LN) -sf $(LINUX_HEADERS_DIR)/include/linux/dvb-git/$(LINUX_HEADERS_GIT_SUBDIR) $(LINUX_HEADERS_DIR)/include/linux/dvb
	$(CP) -RPp $(LINUX_HEADERS_DIR)/include/* \
		$(LINUX_HEADERS_INSTALL_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.linux_headers_installed

.PHONY: clean-linux-headers distclean-linux-headers

clean-linux-headers:
	-$(RM) -rf $(LINUX_HEADERS_DIR)

distclean-linux-headers:
	-$(RM) -f $(STAGEFILES_DIR)/.linux_headers_unpacked
