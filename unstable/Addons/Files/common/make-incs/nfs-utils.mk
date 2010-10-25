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

ifeq ($(CONFIG_NFS-UTILS),y)
ifneq ($(CONFIG_TCP_WRAPPERS),y)
   $(error dependency error: nfs-utils needs tcp_wrappers enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
NFS-UTILS_DEPS = $(BASE_BUILD_STAGEFILE) $(TCP_WRAPPERS_INSTALLED)

NFS-UTILS_VERSION := 1.1.6
NFS-UTILS_PATCHES_DIR := $(PATCHES_DIR)/nfs-utils/$(NFS-UTILS_VERSION)

NFS-UTILS_FILE := nfs-utils-$(NFS-UTILS_VERSION).tar.bz2
NFS-UTILS_DLFILE := $(DOWNLOAD_DIR)/$(NFS-UTILS_FILE)
NFS-UTILS_URL := http://downloads.sourceforge.net/nfs/$(NFS-UTILS_FILE)
NFS-UTILS_DIR := $(BUILD_DIR)/nfs-utils-$(NFS-UTILS_VERSION)

NFS-UTILS_INSTALLED = $(STAGEFILES_DIR)/.nfs-utils_installed

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
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_patched

#
# configure nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_configured: $(STAGEFILES_DIR)/.nfs-utils_patched
	($(CD) $(NFS-UTILS_DIR) ; $(UCLIBC_ENV) \
		CC_FOR_BUILD=$(UCLIBC_CC) \
		$(NFS-UTILS_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--disable-uuid \
			--disable-gss \
			--disable-nfsv4 \
			--enable-static \
			--enable-shared)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_configured

#
# compile nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_compiled: $(STAGEFILES_DIR)/.nfs-utils_configured
	$(UCLIBC_ENV) \
	CC_FOR_BUILD=$(UCLIBC_CC) \
	OPT="$(UCLIBC_CFLAGS) -I$(TARGET_ROOT)/lib " \
	INSTALLSUID="install -m 4755" \
	DESTDIR="$(TARGET_ROOT)" \
	$(MAKE) -C $(NFS-UTILS_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_compiled

#
# install nfs-utils
#

$(STAGEFILES_DIR)/.nfs-utils_installed: $(STAGEFILES_DIR)/.nfs-utils_compiled
	$(CP) -f $(NFS-UTILS_DIR)/utils/statd/sm-notify $(TARGET_ROOT)/usr/sbin/sm-notify
	$(CP) -f $(NFS-UTILS_DIR)/utils/statd/statd $(TARGET_ROOT)/usr/sbin/rpc.statd
	$(CP) -f $(NFS-UTILS_DIR)/utils/nfsd/nfsd $(TARGET_ROOT)/usr/sbin/rpc.nfsd
	$(CP) -f $(NFS-UTILS_DIR)/utils/mount/mount.nfs $(TARGET_ROOT)/bin/mount.nfs
	$(CP) -f $(NFS-UTILS_DIR)/utils/mountd/mountd $(TARGET_ROOT)/usr/sbin/rpc.mountd
	$(CP) -f $(NFS-UTILS_DIR)/utils/exportfs/exportfs $(TARGET_ROOT)/usr/sbin/exportfs
	$(CP) -f $(NFS-UTILS_DIR)/utils/showmount/showmount $(TARGET_ROOT)/usr/sbin/showmount
	$(CP) -f $(NFS-UTILS_DIR)/utils/nfsstat/nfsstat $(TARGET_ROOT)/usr/sbin/nfsstat
	$(TOUCH) $(STAGEFILES_DIR)/.nfs-utils_installed


$(FILELIST_DIR)/nfs-utils.lst: $(STAGEFILES_DIR)/.nfs-utils_installed
	$(TOUCH) $(FILELIST_DIR)/nfs-utils.lst

.PHONY: clean-nfs-utils distclean-nfs-utils

clean-nfs-utils:
	-$(RM) -rf $(NFS-UTILS_DIR)

#
# clean everthing else
#

distclean-nfs-utils:
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_patched
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_configured
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.nfs-utils_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NFS-UTILS_DLFILE)
endif
