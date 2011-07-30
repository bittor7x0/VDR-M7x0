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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
CURL_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_ZLIB),y)
	CURL_DEPS +=  $(ZLIB_INSTALLED)
endif

CURL_VERSION := 7.19.6
CURL_PATCHES_DIR := $(PATCHES_DIR)/curl/$(CURL_VERSION)

CURL_FILE := curl-$(CURL_VERSION).tar.bz2
CURL_DLFILE := $(DOWNLOAD_DIR)/$(CURL_FILE)
CURL_URL := http://curl.haxx.se/download/$(CURL_FILE)
CURL_DIR := $(BUILD_DIR)/curl-$(CURL_VERSION)

CURL_INSTALLED = $(STAGEFILES_DIR)/.curl_installed

PACKS_RULES_$(CONFIG_CURL) += $(CURL_INSTALLED)
FILE_LISTS_$(CONFIG_CURL) += curl.lst

CLEAN_RULES += clean-curl
DISTCLEAN_RULES += distclean-curl

#
# download curl
#

$(CURL_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CURL_DLFILE) ] ; then \
	$(WGET) $(CURL_URL) -O $(CURL_DLFILE) ; \
	fi );
	$(TOUCH) $(CURL_DLFILE)

#
# unpack curl
#

$(STAGEFILES_DIR)/.curl_unpacked: $(CURL_DLFILE) \
                                           $(wildcard $(CURL_PATCHES_DIR)/*.patch) \
                                           $$(CURL_DEPS)
	-$(RM) -rf $(CURL_DIR)
	$(BZCAT) $(CURL_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.curl_unpacked

#
# patch curl
#

$(STAGEFILES_DIR)/.curl_patched: $(STAGEFILES_DIR)/.curl_unpacked
	$(call patch_package, $(CURL_DIR), $(CURL_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.curl_patched

#
# configure curl
#

$(STAGEFILES_DIR)/.curl_configured: $(STAGEFILES_DIR)/.curl_patched
	($(CD) $(CURL_DIR) ; $(UCLIBC_ENV) \
		LDFLAGS="$(UCLIBC_LDFLAGS) -L$(TARGET_ROOT)/lib -L$(TARGET_ROOT)/usr/lib -Wl,-rpath-link=$(TARGET_ROOT)/usr/lib $(if $(CONFIG_ZLIB),-lz)" \
		$(CURL_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--disable-thread \
			--enable-cookies \
			--disable-crypto-auth \
			--enable-nonblocking \
			--disable-file \
			--disable-ftp \
			--enable-http \
			--disable-ares \
			--disable-debug \
			--disable-dict \
			--disable-gopher \
			--disable-ipv6 \
			--disable-ldap \
			--disable-ldaps \
			--disable-manual \
			--disable-proxy \
			--disable-sspi \
			--disable-telnet \
			--disable-tftp \
			--disable-verbose \
			--with-random="/dev/urandom" \
			--without-ssl \
			--without-ca-bundle \
			--without-gnutls \
			--without-krb4 \
			--without-libidn \
			--without-nss \
			--without-spnego \
			--without-gssapi \
			--without-libssh2 \
			$(if $(CONFIG_ZLIB),--with-zlib="$(TARGET_ROOT)/usr",--without-zlib))
	$(TOUCH) $(STAGEFILES_DIR)/.curl_configured

#
# compile curl
#

$(STAGEFILES_DIR)/.curl_compiled: $(STAGEFILES_DIR)/.curl_configured
	$(MAKE) -C $(CURL_DIR) $(UCLIBC_ENV)
	$(TOUCH) $(STAGEFILES_DIR)/.curl_compiled

#
# install curl
#

$(STAGEFILES_DIR)/.curl_installed: $(STAGEFILES_DIR)/.curl_compiled
	$(MAKE) -C $(CURL_DIR) $(UCLIBC_ENV) install
	$(TOUCH) $(STAGEFILES_DIR)/.curl_installed


$(FILELIST_DIR)/curl.lst: $(STAGEFILES_DIR)/.curl_installed
	$(TOUCH) $(FILELIST_DIR)/curl.lst

.PHONY: clean-curl distclean-curl

clean-curl:
	-$(RM) -rf $(CURL_DIR)

#
# clean everthing else
#

distclean-curl:
	-$(RM) -f $(STAGEFILES_DIR)/.curl_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.curl_patched
	-$(RM) -f $(STAGEFILES_DIR)/.curl_configured
	-$(RM) -f $(STAGEFILES_DIR)/.curl_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.curl_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CURL_DLFILE)
endif
