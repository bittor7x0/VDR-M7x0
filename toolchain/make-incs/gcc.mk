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
# $Id: gcc.mk 403 2009-08-11 22:40:16Z andreas $
#

GCC_VERSION := 4.5.1
GCC_PATCHES_DIR := $(PATCHES_DIR)/gcc/$(GCC_VERSION)

GCC_FILE1 := gcc-core-$(GCC_VERSION).tar.bz2
GCC_DLFILE1 := $(DOWNLOAD_DIR)/$(GCC_FILE1)
GCC_FILE2 := gcc-g++-$(GCC_VERSION).tar.bz2
GCC_DLFILE2 := $(DOWNLOAD_DIR)/$(GCC_FILE2)
GCC_DIR := $(BUILD_DIR)/gcc-$(GCC_VERSION)
GCC_URL1 := ftp://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/$(GCC_FILE1)
GCC_URL2 := ftp://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/$(GCC_FILE2)
GCC_BUILD_DIR := $(BUILD_DIR)/gcc.build
GCC_STAGE1_BUILD_DIR := $(BUILD_DIR)/gcc.stage1.build

GCC_STAGE1_PREFIX = $(BUILD_DIR)/gcc.stage1
GCC_STAGE1_PREFIX_BIN = $(GCC_STAGE1_PREFIX)/bin


GCC_STAGE1_INSTALLED = $(STAGEFILES_DIR)/.gcc_stage1_installed
GCC_INSTALLED = $(STAGEFILES_DIR)/.gcc_installed

BASE_RULES_y += $(GCC_INSTALLED)
CLEAN_RULES += clean-gcc
DISTCLEAN_RULES += distclean-gcc
FILE_LISTS_y += gcc.lst

#
# download gcc
#
$(GCC_DLFILE1) $(GCC_DLFILE2): $(TC_INIT_RULE)
	(if [ ! -f $(GCC_DLFILE1) ] ; then \
	$(WGET) $(GCC_URL1) -O $(GCC_DLFILE1) ; \
	fi );
	$(TOUCH) $(GCC_DLFILE1) ;
	(if [ ! -f $(GCC_DLFILE2) ] ; then \
	$(WGET) $(GCC_URL2) -O $(GCC_DLFILE2) ; \
	fi );
	$(TOUCH) $(GCC_DLFILE2) ;

#
# unpack gcc
#

$(STAGEFILES_DIR)/.gcc_unpacked: $(GCC_DLFILE1) $(GCC_DLFILE2) $(GCC_PATCHES_DIR)/*.patch
	-$(RM) -rf $(GCC_DIR)
	$(BZCAT) $(GCC_DLFILE1) | $(TAR) -C $(BUILD_DIR) -f -
	$(BZCAT) $(GCC_DLFILE2) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_unpacked
#
# patch gcc
#

$(STAGEFILES_DIR)/.gcc_patched: $(STAGEFILES_DIR)/.gcc_unpacked
	$(call patch_package, $(GCC_DIR), $(GCC_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_patched

#
# configure gcc-stage1
#

$(STAGEFILES_DIR)/.gcc_stage1_configured: $(STAGEFILES_DIR)/.gcc_patched \
                                          $$(BINUTILS_INSTALLED) \
                                          $$(MPC_INSTALLED) \
                                          $$(UCLIBC_PRE_ALL_GCC)
	-$(RM) -rf $(GCC_STAGE1_BUILD_DIR)
	$(MKDIR) -p $(GCC_STAGE1_BUILD_DIR)
	($(CD) $(GCC_STAGE1_BUILD_DIR) ; PATH='$(PREFIX_BIN):$(PATH)' \
		$(GCC_DIR)/configure \
			--prefix=$(GCC_STAGE1_PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--with-as=$(PREFIX_BIN)/$(UCLIBC_AS) \
			--with-gnu-as \
			--with-ld=$(PREFIX_BIN)/$(UCLIBC_LD) \
			--with-gnu-ld \
			--with-system-zlib \
			--target=$(UCLIBC_TARGET) \
			--enable-languages=c \
			--enable-__cxa_atexit \
			--disable-shared \
			--enable-threads=posix \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libmudflap \
			--nfp \
			--enable-multilib \
			--enable-softfloat \
			--disable-nls \
			--disable-tls )
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_stage1_configured

#
# compile gcc-stage1
#

$(STAGEFILES_DIR)/.gcc_stage1_compiled: $(STAGEFILES_DIR)/.gcc_stage1_configured
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_STAGE1_BUILD_DIR) all-gcc all-target-libgcc
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_stage1_compiled
#
# install gcc-stage1
#

$(STAGEFILES_DIR)/.gcc_stage1_installed: $(STAGEFILES_DIR)/.gcc_stage1_compiled
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_STAGE1_BUILD_DIR) install-gcc install-target-libgcc
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_stage1_installed


#
# configure gcc
#

$(STAGEFILES_DIR)/.gcc_configured: $(STAGEFILES_DIR)/.gcc_patched \
                                   $$(BINUTILS_INSTALLED) \
                                   $$(MPC_INSTALLED) \
                                   $$(UCLIBC_INSTALLED)
	-$(RM) -rf $(GCC_BUILD_DIR)
	$(MKDIR) -p $(GCC_BUILD_DIR)
	($(CD) $(GCC_BUILD_DIR) ; PATH='$(PREFIX_BIN):$(PATH)' \
		$(GCC_DIR)/configure \
			--prefix=$(PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--with-gnu-as \
			--with-gnu-ld \
			--with-system-zlib \
			--target=$(UCLIBC_TARGET) \
			--enable-languages=c,c++ \
			--enable-__cxa_atexit \
			--enable-shared \
			--enable-threads=posix \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libmudflap \
			--nfp \
			--enable-multilib \
			--enable-softfloat \
			--disable-nls \
			--disable-tls )
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_configured

#
# compile gcc
#

$(STAGEFILES_DIR)/.gcc_compiled: $(STAGEFILES_DIR)/.gcc_configured
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_compiled

#
# install gcc
#

$(STAGEFILES_DIR)/.gcc_installed: $(STAGEFILES_DIR)/.gcc_compiled
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_BUILD_DIR) install
	$(CP) -Pp '$(PREFIX)/$(UCLIBC_TARGET)/lib/lib'* '$(TARGET_ROOT)/lib'
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_installed

$(FILELIST_DIR)/gcc.lst: $(STAGEFILES_DIR)/.gcc_installed
	$(TOUCH) $(FILELIST_DIR)/gcc.lst

.PHONY: clean-gcc distclean-gcc

clean-gcc:
	-$(RM) -rf $(GCC_BUILD_DIR)
	-$(RM) -rf $(GCC_STAGE1_BUILD_DIR)
	-$(RM) -rf $(GCC_DIR)

distclean-gcc:
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_stage1_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_stage1_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_stage1_installed
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gcc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GCC_DLFILE)
endif
