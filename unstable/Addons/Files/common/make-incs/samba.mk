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
SAMBA_DEPS = $(BASE_BUILD_STAGEFILE)

# Version
SAMBA_VERSION := 2.2.12
# Patches dir
SAMBA_PATCHES_DIR := $(PATCHES_DIR)/samba/$(SAMBA_VERSION)


SAMBA_FILE := samba-$(SAMBA_VERSION).tar.gz
SAMBA_DLFILE := $(DOWNLOAD_DIR)/$(SAMBA_FILE)
SAMBA_URL := http://us1.samba.org/samba/ftp/stable/$(SAMBA_FILE)
SAMBA_DIR := $(BUILD_DIR)/samba-$(SAMBA_VERSION)

IVARS = BASEDIR=$(TARGET_ROOT)/usr \
	BINDIR=$(TARGET_ROOT)/usr/bin \
	SBINDIR=$(TARGET_ROOT)/usr/sbin \
	MANDIR=$(TARGET_ROOT)/usr/share/man \
	LIBDIR=$(TARGET_ROOT)/etc/samba \
	VARDIR=$(TARGET_ROOT)/var \
	PIDDIR=$(TARGET_ROOT)/var/run \
	INCLUDEDIR=$(TARGET_ROOT)/usr/include \
	SWATDIR=$(TARGET_ROOT)/usr/share/samba/swat \
	CODEPAGEDIR=$(TARGET_ROOT)/etc/samba/codepages

SAMBA_INSTALLED = $(STAGEFILES_DIR)/.samba_installed

PACKS_RULES_$(CONFIG_SAMBA) += $(SAMBA_INSTALLED)
FILE_LISTS_$(CONFIG_SAMBA) += samba.lst

CLEAN_RULES += clean-samba
DISTCLEAN_RULES += distclean-samba

#
# download samba
#

$(SAMBA_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SAMBA_DLFILE) ] ; then \
	$(WGET) $(SAMBA_URL) -O $(SAMBA_DLFILE) ; \
	fi );
	$(TOUCH) $(SAMBA_DLFILE)

#
# unpack samba
#

#$(STAGEFILES_DIR)/.samba_unpacked: $(SAMBA_DLFILE) \
#                                       $(wildcard $(SAMBA_PATCHES_DIR)/*.patch) \
#                                       $(SAMBA_DEPS)
$(STAGEFILES_DIR)/.samba_unpacked: $(SAMBA_DLFILE) \
                                       $(SAMBA_DEPS)
	-$(RM) -rf $(SAMBA_DIR)
	$(GZIP) -d -c $(SAMBA_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.samba_unpacked

#
# patch samba
#

$(STAGEFILES_DIR)/.samba_patched: $(STAGEFILES_DIR)/.samba_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(SAMBA_DIR) $(SAMBA_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.samba_patched

#
# configure package
# if possible in an own build dir.
# dependencies should put here, second expansion is needed.
# all packages should install somewhere below $(TARGET_ROOT)
# and configured for $(TARGET) (or $(UCLIBC_TARGET) if this works)
# $(UCLIBC_ENV) setups some environment variables as CFLAGS, PATH ...

$(STAGEFILES_DIR)/.samba_configured: $(STAGEFILES_DIR)/.samba_patched
	($(CD) $(SAMBA_DIR)/source ; $(UCLIBC_ENV) \
		ac_cv_sizeof_int=4 \
		ac_cv_sizeof_long=4 \
		ac_cv_sizeof_short=2 \
		samba_cv_have_setresuid=yes \
		samba_cv_have_setresgid=yes \
		samba_cv_have_longlong=yes \
		samba_cv_SIZEOF_OFF_T=yes \
		samba_cv_HAVE_OFF64_T=no \
		samba_cv_SIZEOF_INO_T=yes \
		samba_cv_HAVE_INO64_T=no \
		samba_cv_HAVE_DEV64_T=no \
		samba_cv_HAVE_STRUCT_DIRENT64=yes \
		samba_cv_HAVE_DEVICE_MAJOR_FN=yes \
		samba_cv_HAVE_DEVICE_MINOR_FN=yes \
		samba_cv_HAVE_MAKEDEV_FN=yes \
		samba_cv_HAVE_UNSIGNED_CHAR=no \
		samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
		samba_cv_HAVE_C99_VSNPRINTF=yes \
		samba_cv_HAVE_BROKEN_READDIR=no \
		samba_cv_HAVE_KERNEL_OPLOCKS_LINUX=yes \
		samba_cv_HAVE_KERNEL_CHANGE_NOTIFY=yes \
		samba_cv_HAVE_KERNEL_SHARE_MODES=yes \
		samba_cv_HAVE_IRIX_SPECIFIC_CAPABILITIES=no \
		samba_cv_HAVE_FTRUNCATE_EXTEND=no \
		samba_cv_HAVE_BROKEN_GETGROUPS=no \
		samba_cv_REPLACE_INET_NTOA=no\
		samba_cv_HAVE_SECURE_MKSTEMP=yes \
		samba_cv_SYSCONF_SC_NGROUPS_MAX=yes \
		samba_cv_HAVE_IFACE_AIX=no \
		samba_cv_HAVE_IFACE_IFCONF=yes \
		samba_cv_USE_SETRESUID=yes \
		samba_cv_HAVE_MMAP=yes \
		samba_cv_FTRUNCATE_NEEDS_ROOT=no \
		samba_cv_HAVE_FCNTL_LOCK=yes \
		samba_cv_HAVE_BROKEN_FCNTL64_LOCKS=no \
		samba_cv_HAVE_STRUCT_FLOCK64=yes \
		samba_cv_HAVE_TRUNCATED_SALT=no \
		fu_cv_sys_stat_statvfs64=yes \
		samba_cv_HAVE_EXPLICIT_LARGEFILE_SUPPORT=yes \
		samba_cv_have_getgroups_too_many_egids=no \
		$(SAMBA_DIR)/source/configure \
			--host=$(TARGET) \
			--build=i486-linux-gnu \
			--prefix=/usr \
			--sysconfdir=/etc \
			--localstatedir=/var \
			--with-privatedir=/etc/samba \
			--with-lockdir=/var/lock \
			--with-piddir=/var/run \
			--with-swatdir=/usr/share/samba/swat \
			--with-configdir=/etc/samba \
			--with-codepagedir=/etc/samba/codepages \
			--with-logfilebase=/var/log \
			--disable-cups \
			--without-winbind \
			)
	$(TOUCH) $(STAGEFILES_DIR)/.samba_configured

#
# compile samba
#

$(STAGEFILES_DIR)/.samba_compiled: $(STAGEFILES_DIR)/.samba_configured
	$(UCLIBC_ENV) $(MAKE) -C $(SAMBA_DIR)/source
	$(TOUCH) $(STAGEFILES_DIR)/.samba_compiled

#
# install samba
#

$(STAGEFILES_DIR)/.samba_installed: $(STAGEFILES_DIR)/.samba_compiled
	$(CP) $(SAMBA_DIR)/source/bin/nmbd $(TARGET_ROOT)/usr/sbin/nmbd
	chmod 755 $(TARGET_ROOT)/usr/sbin/nmbd
	$(CP) $(SAMBA_DIR)/source/bin/smbd $(TARGET_ROOT)/usr/sbin/smbd
	chmod 755 $(TARGET_ROOT)/usr/sbin/smbd
	$(CP) $(SAMBA_DIR)/source/bin/smbpasswd $(TARGET_ROOT)/usr/sbin/smbpasswd
	chmod 755 $(TARGET_ROOT)/usr/sbin/smbpasswd
	$(MKDIR) -p $(TARGET_ROOT)/etc/samba
	$(CP) $(SAMBA_DIR)/source/bin/smb.conf $(TARGET_ROOT)/etc/samba/smb.conf
	$(MKDIR) -p $(TARGET_ROOT)/usr/share
	$(MKDIR) -p $(TARGET_ROOT)/usr/share/samba
	$(BZCAT) $(SAMBA_PATCHES_DIR)/samba.codepages.tar.bz2 | \
		 $(TAR) -C $(TARGET_ROOT)/usr/share/samba/ -f -
	( if [ X"`$(GREP) netbios-ns $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services`" == X"" ] ; then \
		$(SED) -i '$$anetbios-ns\t137/udp' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services ; \
	fi );
	(if [ X"`$(GREP) netbios-ns $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf`" == X"" ] ; then \
		$(SED) -i '$$anetbios-ns\tdgram\tudp\twait\troot\t/usr/sbin/nmbd nmbd' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf ; \
	fi );
	(if [ X"`$(GREP) netbios-ssn $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services`" == X"" ] ; then \
		$(SED) -i '$$anetbios-ssn\t139/tcp' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services ; \
	fi );
	(if [ X"`$(GREP) netbios-ssn $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf`" == X"" ] ; then \
		$(SED) -i '$$anetbios-ssn\tstream\ttcp\tnowait\troot\t/usr/sbin/smbd smbd' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf ; \
	fi );
	(if [ X"`$(GREP) microsoft-ds $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services`" == X"" ] ; then \
		$(SED) -i '$$amicrosoft-ds\t445/tcp' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/services ; \
	fi );
	(if [ X"`$(GREP) microsoft-ds $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf`" == X"" ] ; then \
		$(SED) -i '$$amicrosoft-ds\tstream\ttcp\tnowait\troot\t/usr/sbin/smbd smbd' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf ; \
	fi );
	$(TOUCH) $(STAGEFILES_DIR)/.samba_installed

$(FILELIST_DIR)/samba.lst: $(STAGEFILES_DIR)/.samba_installed
	$(TOUCH) $(FILELIST_DIR)/samba.lst

.PHONY: clean-samba distclean-samba


clean-samba:
	-$(RM) -rf $(SAMBA_DIR)

distclean-samba:
	-$(RM) -f $(STAGEFILES_DIR)/.samba_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.samba_patched
	-$(RM) -f $(STAGEFILES_DIR)/.samba_configured
	-$(RM) -f $(STAGEFILES_DIR)/.samba_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.samba_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SAMBA_DLFILE)
endif
