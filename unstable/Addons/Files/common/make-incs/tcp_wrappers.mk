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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
TCP_WRAPPERS_DEPS = $(BASE_BUILD_STAGEFILE)

TCP_WRAPPERS_VERSION := 7.6
TCP_WRAPPERS_PATCHES_DIR := $(PATCHES_DIR)/tcp_wrappers/$(TCP_WRAPPERS_VERSION)

TCP_WRAPPERS_FILE := tcp_wrappers_$(TCP_WRAPPERS_VERSION).tar.gz
TCP_WRAPPERS_DLFILE := $(DOWNLOAD_DIR)/$(TCP_WRAPPERS_FILE)
TCP_WRAPPERS_URL := ftp://ftp.porcupine.org/pub/security/$(TCP_WRAPPERS_FILE)
TCP_WRAPPERS_DIR := $(BUILD_DIR)/tcp_wrappers_$(TCP_WRAPPERS_VERSION)

TCP_WRAPPERS_INSTALLED = $(STAGEFILES_DIR)/.tcp_wrappers_installed

PACKS_RULES_$(CONFIG_TCP_WRAPPERS) += $(TCP_WRAPPERS_INSTALLED)
FILE_LISTS_$(CONFIG_TCP_WRAPPERS) += tcp_wrappers.lst

CLEAN_RULES += clean-tcp_wrappers
DISTCLEAN_RULES += distclean-tcp_wrappers

#
# download tcp_wrappers
#

$(TCP_WRAPPERS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(TCP_WRAPPERS_DLFILE) ] ; then \
	$(WGET) $(TCP_WRAPPERS_URL) -O $(TCP_WRAPPERS_DLFILE) ; \
	fi );
	$(TOUCH) $(TCP_WRAPPERS_DLFILE)

#
# unpack tcp_wrappers
#

$(STAGEFILES_DIR)/.tcp_wrappers_unpacked: $(TCP_WRAPPERS_DLFILE) \
                                           $(wildcard $(TCP_WRAPPERS_PATCHES_DIR)/*.patch) \
                                           $$(TCP_WRAPPERS_DEPS)
	-$(RM) -rf $(TCP_WRAPPERS_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(TCP_WRAPPERS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.tcp_wrappers_unpacked

#
# patch tcp_wrappers
#

$(STAGEFILES_DIR)/.tcp_wrappers_patched: $(STAGEFILES_DIR)/.tcp_wrappers_unpacked
	$(call patch_package, $(TCP_WRAPPERS_DIR), $(TCP_WRAPPERS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.tcp_wrappers_patched

#
# compile tcp_wrappers
#

$(STAGEFILES_DIR)/.tcp_wrappers_compiled: $(STAGEFILES_DIR)/.tcp_wrappers_patched
	$(MAKE) -C $(TCP_WRAPPERS_DIR) \
		PATH="$(PREFIX_BIN):$(PATH)" CC="$(UCLIBC_CC)" \
		CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
		RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" STRIP="$(UCLIBC_STRIP)" \
		OPT_CFLAGS="$(UCLIBC_CFLAGS) -fPIC" \
		LIBS=-lnsl \
		NETGROUP= \
		VSYSLOG= \
		BUGS= \
		EXTRA_CFLAGS="-DSYS_ERRLIST_DEFINED -DHAVE_STRERROR -DHAVE_WEAKSYMS -D_REENTRANT -DINET6=1 \
			-Dss_family=__ss_family -Dss_len=__ss_len" \
		FACILITY=LOG_DAEMON \
		SEVERITY=LOG_INFO \
		REAL_DAEMON_DIR=/usr/sbin \
		STYLE="-DPROCESS_OPTIONS" \
		tidy all
	$(TOUCH) $(STAGEFILES_DIR)/.tcp_wrappers_compiled

#
# install tcp_wrappers
#

$(STAGEFILES_DIR)/.tcp_wrappers_installed: $(STAGEFILES_DIR)/.tcp_wrappers_compiled
	$(CP) -f $(TCP_WRAPPERS_DIR)/tcpd.h $(TARGET_ROOT)/usr/include/
	$(CP) -f $(TCP_WRAPPERS_DIR)/libwrap.a $(TARGET_ROOT)/usr/lib/
	$(CP) -f $(TCP_WRAPPERS_DIR)/shared/libwrap.so* $(TARGET_ROOT)/usr/lib/
	$(TOUCH) $(STAGEFILES_DIR)/.tcp_wrappers_installed


$(FILELIST_DIR)/tcp_wrappers.lst: $(STAGEFILES_DIR)/.tcp_wrappers_installed
	$(TOUCH) $(FILELIST_DIR)/tcp_wrappers.lst

.PHONY: clean-tcp_wrappers distclean-tcp_wrappers

clean-tcp_wrappers:
	-$(RM) -rf $(TCP_WRAPPERS_DIR)

#
# clean everthing else
#

distclean-tcp_wrappers:
	-$(RM) -f $(STAGEFILES_DIR)/.tcp_wrappers_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.tcp_wrappers_patched
	-$(RM) -f $(STAGEFILES_DIR)/.tcp_wrappers_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.tcp_wrappers_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(TCP_WRAPPERS_DLFILE)
endif
