# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2019 VDR-NG-EM Project
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
# The project's page is at http://www.assembla.com/spaces/VDR-M7x0
# Our English forum is at  http://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  http://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
MINI-SENDMAIL_DEPS = $(BASE_BUILD_STAGEFILE)

MINI-SENDMAIL_VERSION := 1.3.6
MINI-SENDMAIL_PATCHES_DIR := $(PATCHES_DIR)/mini-sendmail/$(MINI-SENDMAIL_VERSION)

MINI-SENDMAIL_FILE := mini_sendmail-$(MINI-SENDMAIL_VERSION).tar.gz
MINI-SENDMAIL_DLFILE := $(DOWNLOAD_DIR)/$(MINI-SENDMAIL_FILE)
MINI-SENDMAIL_URL := http://www.acme.com/software/mini_sendmail/$(MINI-SENDMAIL_FILE)
MINI-SENDMAIL_DIR := $(BUILD_DIR)/mini_sendmail-$(MINI-SENDMAIL_VERSION)

MINI-SENDMAIL_INSTALLED = $(STAGEFILES_DIR)/.mini-sendmail_installed

PACKS_RULES_$(CONFIG_MINI-SENDMAIL) += $(MINI-SENDMAIL_INSTALLED)
FILE_LISTS_$(CONFIG_MINI-SENDMAIL) += mini-sendmail.lst

CLEAN_RULES += clean-mini-sendmail
DISTCLEAN_RULES += distclean-mini-sendmail

#
# download mini-sendmail
#

$(MINI-SENDMAIL_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MINI-SENDMAIL_DLFILE) ] ; then \
	$(WGET) $(MINI-SENDMAIL_URL) -O $(MINI-SENDMAIL_DLFILE) ; \
	fi );
	$(TOUCH) $(MINI-SENDMAIL_DLFILE)

#
# unpack mini-sendmail
#

$(STAGEFILES_DIR)/.mini-sendmail_unpacked: $(MINI-SENDMAIL_DLFILE) \
                                           $(wildcard $(MINI-SENDMAIL_PATCHES_DIR)/*.patch) \
                                           $$(MINI-SENDMAIL_DEPS)
	-$(RM) -rf $(MINI-SENDMAIL_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(MINI-SENDMAIL_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mini-sendmail_unpacked

#
# patch mini-sendmail
#

$(STAGEFILES_DIR)/.mini-sendmail_patched: $(STAGEFILES_DIR)/.mini-sendmail_unpacked
	$(call patch_package, $(MINI-SENDMAIL_DIR), $(MINI-SENDMAIL_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mini-sendmail_patched

#
# compile mini-sendmail
#

$(STAGEFILES_DIR)/.mini-sendmail_compiled: $(STAGEFILES_DIR)/.mini-sendmail_patched
	$(UCLIBC_ENV) $(MAKE) $(UCLIBC_ENV) \
		BINDIR=$(TARGET_ROOT)/usr/sbin \
		MANDIR=$(TARGET_ROOT)/usr/sbin \
		-C $(MINI-SENDMAIL_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.mini-sendmail_compiled

#
# install mini-sendmail
#

$(STAGEFILES_DIR)/.mini-sendmail_installed: $(STAGEFILES_DIR)/.mini-sendmail_compiled
	$(UCLIBC_ENV) $(MAKE) $(UCLIBC_ENV) \
		BINDIR=$(TARGET_ROOT)/usr/sbin \
		MANDIR=$(TARGET_ROOT)/usr/man \
		-C $(MINI-SENDMAIL_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.mini-sendmail_installed


$(FILELIST_DIR)/mini-sendmail.lst: $(STAGEFILES_DIR)/.mini-sendmail_installed
	$(TOUCH) $(FILELIST_DIR)/mini-sendmail.lst

.PHONY: clean-mini-sendmail distclean-mini-sendmail

clean-mini-sendmail:
	-$(RM) -rf $(MINI-SENDMAIL_DIR)

#
# clean everthing else
#

distclean-mini-sendmail:
	-$(RM) -f $(STAGEFILES_DIR)/.mini-sendmail_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mini-sendmail_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mini-sendmail_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mini-sendmail_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MINI-SENDMAIL_DLFILE)
endif
