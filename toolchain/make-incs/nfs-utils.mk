# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
NFS-UTILS_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_LIBTIRPC),y)
	NFS-UTILS_DEPS +=  $(LIBTIRPC_INSTALLED)
endif
ifeq ($(CONFIG_TCP_WRAPPERS),y)
	NFS-UTILS_DEPS +=  $(TCP_WRAPPERS_INSTALLED)
endif

NFS-UTILS_VERSION := 1.3.4
NFS-UTILS_PATCHES_DIR := $(PATCHES_DIR)/nfs-utils/$(NFS-UTILS_VERSION)

NFS-UTILS_FILE := nfs-utils-$(NFS-UTILS_VERSION).tar.bz2
NFS-UTILS_DLFILE := $(DOWNLOAD_DIR)/$(NFS-UTILS_FILE)
NFS-UTILS_URL := http://downloads.sourceforge.net/nfs/$(NFS-UTILS_FILE)
NFS-UTILS_DIR := $(BUILD_DIR)/nfs-utils-$(NFS-UTILS_VERSION)
NFS-UTILS_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/nfs-utils-$(NFS-UTILS_VERSION)

NFS-UTILS_INSTALLED = $(STAGEFILES_DIR)/.nfs-utils_host_installed $(STAGEFILES_DIR)/.nfs-utils_installed

PACKS_RULES_$(CONFIG_NFS-UTILS) += $(NFS-UTILS_INSTALLED)
FILE_LISTS_$(CONFIG_NFS-UTILS) += nfs-utils.lst

CLEAN_RULES += clean-nfs-utils
DISTCLEAN_RULES += distclean-nfs-utils

#
# download nfs-utils
#

$(NFS-UTILS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NFS-UTILS_DLFILE) ] ; then \
	$(WGET) $(NFS-UTILS_URL) -O $(NFS-UTILS_DLFILE) ; \
	fi );
	$(TOUCH) $(NFS-UTILS_DLFILE)

#
# unpack nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_unpacked: $(NFS-UTILS_DLFILE) \
                                           $(wildcard $(NFS-UTILS_PATCHES_DIR)/*.patch) \
                                           $$(NFS-UTILS_DEPS)
	-$(RM) -rf $(NFS-UTILS_DIR)
	$(BZCAT) $(NFS-UTILS_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_unpacked

#
# patch nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_patched: $(STAGEFILES_DIR)/.nfs-utils_unpacked
	$(call patch_package, $(NFS-UTILS_DIR), $(NFS-UTILS_PATCHES_DIR))
	-$(RM) -rf $(NFS-UTILS_HOSTDIR)
	$(CP) -RPp $(NFS-UTILS_DIR) $(HOSTUTILS_BUILD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_patched

#
# configure nfs-utils in host
#

$(STAGEFILES_DIR)/.nfs-utils_host_configured: $(STAGEFILES_DIR)/.nfs-utils_patched
	($(CD) $(NFS-UTILS_HOSTDIR) ; \
		$(NFS-UTILS_HOSTDIR)/configure \
			--prefix=$(HOSTUTILS_PREFIX) \
			--disable-shared \
			--enable-static \
			--disable-uuid \
			--disable-caps \
			--disable-gss \
			--disable-nfsdcltrack \
			--disable-nfsv4 \
			--disable-nfsv41 \
			--disable-ipv6 \
			--with-statduser=nobody \
			--without-tcp-wrappers \
			--with-rpcgen=internal)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_host_configured

#
# compile nfs-utils in host
#

$(STAGEFILES_DIR)/.nfs-utils_host_compiled: $(STAGEFILES_DIR)/.nfs-utils_host_configured
	$(MAKE) -C $(NFS-UTILS_HOSTDIR)/tools/rpcgen all
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_host_compiled

#
# install nfs-utils in host
#

$(STAGEFILES_DIR)/.nfs-utils_host_installed: $(STAGEFILES_DIR)/.nfs-utils_host_compiled
	$(CP) -f $(NFS-UTILS_HOSTDIR)/tools/rpcgen/rpcgen $(HOSTUTILS_PREFIX_BIN)/rpcgen
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_host_installed

#
# configure nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_configured: $(STAGEFILES_DIR)/.nfs-utils_host_installed
	($(CD) $(NFS-UTILS_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		CC_FOR_BUILD=$(UCLIBC_CC) \
		$(NFS-UTILS_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-uuid \
			--disable-caps \
			--disable-gss \
			--disable-nfsdcltrack \
			--disable-nfsv4 \
			--disable-nfsv41 \
			--disable-ipv6 \
			--with-statduser=nobody \
			$(if $(CONFIG_LIBTIRPC),--enable-tirpc --with-tirpcinclude=${TARGET_ROOT}/usr/include/tirpc,--disable-tirpc) \
			$(if $(CONFIG_TCP_WRAPPERS),--with-tcp-wrappers,--without-tcp-wrappers) \
			--with-rpcgen=$(HOSTUTILS_PREFIX_BIN)/rpcgen \
			--with-gnu-ld \
			--enable-static \
			--enable-shared)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_configured

#
# compile nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_compiled: $(STAGEFILES_DIR)/.nfs-utils_configured
	$(UCLIBC_ENV_LTO_GC) \
	CC_FOR_BUILD=$(UCLIBC_CC) \
	OPT="$(UCLIBC_CFLAGS_LTO_GC) -I$(TARGET_ROOT)/lib " \
	INSTALLSUID="install -m 4755" \
	DESTDIR="$(TARGET_ROOT)" \
	$(MAKE) -C $(NFS-UTILS_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_compiled

#
# install nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_installed: $(STAGEFILES_DIR)/.nfs-utils_compiled
	#$(CP) -f $(NFS-UTILS_DIR)/utils/statd/sm-notify $(TARGET_ROOT)/usr/sbin/sm-notify
	$(CP) -f $(NFS-UTILS_DIR)/utils/statd/statd $(TARGET_ROOT)/usr/sbin/rpc.statd
	$(CP) -f $(NFS-UTILS_DIR)/utils/nfsd/nfsd $(TARGET_ROOT)/usr/sbin/rpc.nfsd
	#$(CP) -f $(NFS-UTILS_DIR)/utils/mount/mount.nfs $(TARGET_ROOT)/bin/mount.nfs
	$(CP) -f $(NFS-UTILS_DIR)/utils/mountd/mountd $(TARGET_ROOT)/usr/sbin/rpc.mountd
	$(CP) -f $(NFS-UTILS_DIR)/utils/exportfs/exportfs $(TARGET_ROOT)/usr/sbin/exportfs
	#$(CP) -f $(NFS-UTILS_DIR)/utils/showmount/showmount $(TARGET_ROOT)/usr/sbin/showmount
	#$(CP) -f $(NFS-UTILS_DIR)/utils/nfsstat/nfsstat $(TARGET_ROOT)/usr/sbin/nfsstat
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_installed


$(FILELIST_DIR)/nfs-utils.lst: $(STAGEFILES_DIR)/.nfs-utils_host_installed \
				$(STAGEFILES_DIR)/.nfs-utils_installed
	$(TOUCH) $(FILELIST_DIR)/nfs-utils.lst

.PHONY: clean-nfs-utils-host distclean-nfs-utils-host clean-nfs-utils distclean-nfs-utils

clean-nfs-utils-host:
	-$(RM) -rf $(NFS-UTILS_HOSTDIR)

clean-nfs-utils:
	-$(RM) -rf $(NFS-UTILS_DIR)

#
# clean everthing else
#

distclean-nfs-utils-host:
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_patched
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NFS-UTILS_DLFILE)
endif

distclean-nfs-utils:
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_patched
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_configured
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NFS-UTILS_DLFILE)
endif
