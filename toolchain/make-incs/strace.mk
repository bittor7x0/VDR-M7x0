# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007-2022 VDR-NG-EM Project
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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
STRACE_DEPS = $(BASE_BUILD_STAGEFILE) $(LINUX_HEADERS_INSTALLED)

STRACE_VERSION := 4.6
STRACE_PATCHES_DIR := $(PATCHES_DIR)/strace/$(STRACE_VERSION)

STRACE_FILE := strace-$(STRACE_VERSION).tar.xz
STRACE_DLFILE := $(DOWNLOAD_DIR)/$(STRACE_FILE)
STRACE_URL := http://downloads.sourceforge.net/strace/$(STRACE_FILE)
STRACE_DIR := $(BUILD_DIR)/strace-$(STRACE_VERSION)

STRACE_INSTALLED = $(STAGEFILES_DIR)/.strace_installed

PACKS_RULES_$(CONFIG_STRACE) += $(STRACE_INSTALLED)
FILE_LISTS_$(CONFIG_STRACE) += strace.lst

CLEAN_RULES += clean-strace
DISTCLEAN_RULES += distclean-strace

#
# download strace
#

$(STRACE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(STRACE_DLFILE) ] ; then \
	$(WGET) $(STRACE_URL) -O $(STRACE_DLFILE) ; \
	fi );
	$(TOUCH) $(STRACE_DLFILE)

#
# unpack strace
#

$(STAGEFILES_DIR)/.strace_unpacked: $(STRACE_DLFILE) \
                                           $(wildcard $(STRACE_PATCHES_DIR)/*.patch) \
                                           $$(STRACE_DEPS)
	-$(RM) -rf $(STRACE_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(STRACE_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.strace_unpacked

#
# patch strace
#

$(STAGEFILES_DIR)/.strace_patched: $(STAGEFILES_DIR)/.strace_unpacked
	$(call patch_package, $(STRACE_DIR), $(STRACE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.strace_patched

#
# configure strace
#

$(STAGEFILES_DIR)/.strace_configured: $(STAGEFILES_DIR)/.strace_patched
	($(CD) $(STRACE_DIR) ; $(UCLIBC_ENV) \
		CFLAGS="$(UCLIBC_CFLAGS) -I$(TARGET_ROOT)/usr/include" \
		$(STRACE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.strace_configured

#
# compile strace
#

$(STAGEFILES_DIR)/.strace_compiled: $(STAGEFILES_DIR)/.strace_configured
	$(UCLIBC_ENV) $(MAKE) -C $(STRACE_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.strace_compiled

#
# install strace
#

$(STAGEFILES_DIR)/.strace_installed: $(STAGEFILES_DIR)/.strace_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(STRACE_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.strace_installed


$(FILELIST_DIR)/strace.lst: $(STAGEFILES_DIR)/.strace_installed
	$(TOUCH) $(FILELIST_DIR)/strace.lst

.PHONY: clean-strace distclean-strace

clean-strace:
	-$(RM) -rf $(STRACE_DIR)

#
# clean everthing else
#

distclean-strace:
	-$(RM) -f $(STAGEFILES_DIR)/.strace_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.strace_patched
	-$(RM) -f $(STAGEFILES_DIR)/.strace_configured
	-$(RM) -f $(STAGEFILES_DIR)/.strace_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.strace_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(STRACE_DLFILE)
endif
