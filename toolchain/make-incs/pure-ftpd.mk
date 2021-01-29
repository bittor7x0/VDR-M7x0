# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2021 VDR-NG-EM Project
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

PURE-FTPD_DEPS = $(BASE_BUILD_STAGEFILE)

# Version
PURE-FTPD_VERSION := 1.0.22
# Patches dir
PURE-FTPD_PATCHES_DIR := $(PATCHES_DIR)/pure-ftpd/$(PURE-FTPD_VERSION)


PURE-FTPD_FILE := pure-ftpd-$(PURE-FTPD_VERSION).tar.bz2
PURE-FTPD_DLFILE := $(DOWNLOAD_DIR)/$(PURE-FTPD_FILE)
PURE-FTPD_URL := ftp://ftp.pureftpd.org/pub/pure-ftpd/releases/$(PURE-FTPD_FILE)
PURE-FTPD_DIR := $(BUILD_DIR)/pure-ftpd-$(PURE-FTPD_VERSION)

PURE-FTPD_INSTALLED = $(STAGEFILES_DIR)/.pure-ftpd_installed

PACKS_RULES_$(CONFIG_PURE-FTPD) += $(PURE-FTPD_INSTALLED)
FILE_LISTS_$(CONFIG_PURE-FTPD) += pure-ftpd.lst

CLEAN_RULES += clean-pure-ftpd
DISTCLEAN_RULES += distclean-pure-ftpd

#
# download pure-ftpd
#

$(PURE-FTPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PURE-FTPD_DLFILE) ] ; then \
	$(WGET) $(PURE-FTPD_URL) -O $(PURE-FTPD_DLFILE) ; \
	fi );
	$(TOUCH) $(PURE-FTPD_DLFILE)

#
# unpack pure-ftpd
#

$(STAGEFILES_DIR)/.pure-ftpd_unpacked: $(PURE-FTPD_DLFILE) \
                                       $(wildcard $(PURE-FTPD_PATCHES_DIR)/*.patch) \
                                       $(PURE-FTPD_DEPS)
	-$(RM) -rf $(PURE-FTPD_DIR)
	$(BZCAT) $(PURE-FTPD_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.pure-ftpd_unpacked

#
# patch pure-ftpd
#

$(STAGEFILES_DIR)/.pure-ftpd_patched: $(STAGEFILES_DIR)/.pure-ftpd_unpacked
	$(call patch_package, $(PURE-FTPD_DIR), $(PURE-FTPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.pure-ftpd_patched

#
# configure package
# if possible in an own build dir.
# dependencies should put here, second expansion is needed.
# all packages should install somewhere below $(TARGET_ROOT)
# and configured for $(TARGET) (or $(UCLIBC_TARGET) if this works)
# $(UCLIBC_ENV) setups some environment variables as CFLAGS, PATH ...

$(STAGEFILES_DIR)/.pure-ftpd_configured: $(STAGEFILES_DIR)/.pure-ftpd_patched
	($(CD) $(PURE-FTPD_DIR) ; $(UCLIBC_ENV) \
		$(PURE-FTPD_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--sysconfdir=/etc/pure-ftpd \
			--enable-largefile \
			--without-dmalloc \
			--without-standalone \
			--with-inetd \
			--without-capabilities \
			--with-shadow \
			--with-usernames \
			--with-iplogging \
			--without-sendfile \
			--without-humor \
			--without-probe-random-dev \
			--without-minimal \
			--without-sysquotas \
			--without-altlog \
			--with-puredb \
			--without-extauth \
			--without-pam \
			--without-cookie \
			--without-throttling \
			--without-ratios \
			--without-quotas \
			--without-ftpwho \
			--with-largefile \
			--without-welcomemsg \
			--without-uploadscript \
			--without-virtualhosts \
			--without-virtualchroot \
			--without-diraliases \
			--without-nonroot \
			--without-peruserlimits \
			--with-language=english \
			--without-ldap \
			--without-mysql \
			--without-pgsql \
			--without-privsep \
			--without-tls \
			--without-certfile \
			--without-bonjour)
	$(TOUCH) $(STAGEFILES_DIR)/.pure-ftpd_configured

#
# compile pure-ftpd
#

$(STAGEFILES_DIR)/.pure-ftpd_compiled: $(STAGEFILES_DIR)/.pure-ftpd_configured
	$(UCLIBC_ENV) $(MAKE) -C $(PURE-FTPD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.pure-ftpd_compiled

#
# install pure-ftpd
#

$(STAGEFILES_DIR)/.pure-ftpd_installed: $(STAGEFILES_DIR)/.pure-ftpd_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(PURE-FTPD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.pure-ftpd_installed

$(FILELIST_DIR)/pure-ftpd.lst: $(STAGEFILES_DIR)/.pure-ftpd_installed
	$(TOUCH) $(FILELIST_DIR)/pure-ftpd.lst

.PHONY: clean-pure-ftpd distclean-pure-ftpd


clean-pure-ftpd:
	-$(RM) -rf $(PURE-FTPD_DIR)

distclean-pure-ftpd:
	-$(RM) -f $(STAGEFILES_DIR)/.pure-ftpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.pure-ftpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.pure-ftpd_configured
	-$(RM) -f $(STAGEFILES_DIR)/.pure-ftpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.pure-ftpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PURE-FTPD_DLFILE)
endif
