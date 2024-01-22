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

GCC_IS_SNAPSHOT := n

ifeq ($(GCC_IS_SNAPSHOT),y)
  GCC_VERSION := 13-20221009
  GCC_FILE := gcc-$(GCC_VERSION).tar.xz
  GCC_URL := http://www.mirrorservice.org/sites/sourceware.org/pub/gcc/snapshots/$(GCC_VERSION)/$(GCC_FILE)
  GCC_PATCHES_DIR := $(PATCHES_DIR)/gcc/snapshot
  GCC_FILE_LIST += gcc-snapshot.lst
else
  GCC_VERSION := 12.2.0
  GCC_FILE := gcc-$(GCC_VERSION).tar.xz
  GCC_URL := ftp://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/$(GCC_FILE)
  GCC_PATCHES_DIR := $(PATCHES_DIR)/gcc/$(GCC_VERSION)
  GCC_FILE_LIST += gcc.lst
endif

GCC_DLFILE := $(DOWNLOAD_DIR)/$(GCC_FILE)
GCC_DIR := $(BUILD_DIR)/gcc-$(GCC_VERSION)
GCC_BUILD_DIR := $(BUILD_DIR)/gcc.build
GCC_STAGE1_BUILD_DIR := $(BUILD_DIR)/gcc.stage1.build

GCC_STAGE1_PREFIX = $(BUILD_DIR)/gcc.stage1
GCC_STAGE1_PREFIX_BIN = $(GCC_STAGE1_PREFIX)/bin

GCC_STAGE1_INSTALLED = $(STAGEFILES_DIR)/.gcc_stage1_installed
GCC_INSTALLED = $(STAGEFILES_DIR)/.gcc_installed

GCC_CFLAGS_FOR_TARGET := $(filter-out -funroll-loops,$(UCLIBC_CFLAGS_LOOPS))

BASE_RULES_y += $(GCC_INSTALLED)
CLEAN_RULES += clean-gcc
DISTCLEAN_RULES += distclean-gcc
FILE_LISTS_y += $(GCC_FILE_LIST)

#
# download gcc
#
$(GCC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GCC_DLFILE) ] ; then \
	$(WGET) $(GCC_URL) -O $(GCC_DLFILE) ; \
	fi );
	$(TOUCH) $(GCC_DLFILE) ;

#
# unpack gcc
#

$(STAGEFILES_DIR)/.gcc_unpacked: $(GCC_DLFILE) $(wildcard $(GCC_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(GCC_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(GCC_DLFILE)
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
                                          $$(GMP_HOSTINSTALLED) \
                                          $$(MPFR_HOSTINSTALLED) \
                                          $$(MPC_HOSTINSTALLED) \
                                          $$(ISL_HOSTINSTALLED) \
                                          $$(ZSTD_HOSTINSTALLED) \
                                          $$(UCLIBC_PRE_ALL_GCC)
	-$(RM) -rf $(GCC_STAGE1_BUILD_DIR)
	$(MKDIR) -p $(GCC_STAGE1_BUILD_DIR)
	($(CD) $(GCC_STAGE1_BUILD_DIR) ; \
		export libgcc_cv_fixed_point=no ; \
		$(if $(filter n,$(UCLIBC_IS_NG)),export glibcxx_cv_c99_math_tr1=no ;) \
		PATH='$(PREFIX_BIN):$(PATH)' \
		$(GCC_DIR)/configure \
			--prefix=$(GCC_STAGE1_PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--with-as=$(PREFIX_BIN)/$(UCLIBC_AS) \
			--with-gnu-as \
			--with-ld=$(PREFIX_BIN)/$(UCLIBC_LD) \
			--with-gnu-ld \
			--with-newlib \
			--target=$(UCLIBC_TARGET) \
			--enable-languages=c \
			--enable-__cxa_atexit \
			--disable-shared \
			--disable-threads \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-zstd=$(PREFIX) \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--with-isl=$(PREFIX) \
			--disable-isl-version-check \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libquadmath \
			--disable-libssp \
			--disable-libitm \
			--disable-libatomic \
			--disable-libsanitizer \
			--disable-target-libiberty \
			--disable-target-zlib \
			--disable-libstdcxx-verbose \
			--disable-target-libstdc++-v3 \
			--nfp \
			--disable-multilib \
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
                                   $$(GMP_HOSTINSTALLED) \
                                   $$(MPFR_HOSTINSTALLED) \
                                   $$(MPC_HOSTINSTALLED) \
                                   $$(ISL_HOSTINSTALLED) \
                                   $$(ZSTD_HOSTINSTALLED) \
                                   $$(UCLIBC_INSTALLED)
	-$(RM) -rf $(GCC_BUILD_DIR)
	$(MKDIR) -p $(GCC_BUILD_DIR)
	($(CD) $(GCC_BUILD_DIR) ; \
		export libgcc_cv_fixed_point=no ; \
		$(if $(filter n,$(UCLIBC_IS_NG)),export glibcxx_cv_c99_math_tr1=no ;) \
		PATH='$(PREFIX_BIN):$(PATH)' \
		$(GCC_DIR)/configure \
			--prefix=$(PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--with-gnu-as \
			--with-gnu-ld \
			--target=$(UCLIBC_TARGET) \
			--enable-languages=c,c++ \
			--enable-__cxa_atexit \
			--enable-shared \
			--enable-threads=posix \
			--enable-lto \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-zstd=$(PREFIX) \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--with-isl=$(PREFIX) \
			--disable-isl-version-check \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libquadmath \
			--disable-libssp \
			--disable-libitm \
			--disable-libatomic \
			--disable-libsanitizer \
			--disable-target-libiberty \
			--disable-target-zlib \
			--disable-libstdcxx-pch \
			--disable-libstdcxx-verbose \
			--disable-libstdcxx-dual-abi \
			--with-default-libstdcxx-abi=new \
			--nfp \
			--disable-multilib \
			--enable-softfloat \
			--disable-nls \
			--disable-tls )
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_configured

#
# compile gcc
#

$(STAGEFILES_DIR)/.gcc_compiled: $(STAGEFILES_DIR)/.gcc_configured
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_BUILD_DIR) \
		CFLAGS_FOR_TARGET="$(UCLIBC_CFLAGS_GC) $(GCC_CFLAGS_FOR_TARGET)" \
		CXXFLAGS_FOR_TARGET="$(UCLIBC_CXXFLAGS_GC) $(GCC_CFLAGS_FOR_TARGET)" \
		LDFLAGS_FOR_TARGET="$(UCLIBC_LDFLAGS_GC) -lgcc" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_compiled

#
# install gcc
#

$(STAGEFILES_DIR)/.gcc_installed: $(STAGEFILES_DIR)/.gcc_compiled
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_BUILD_DIR) \
		CFLAGS_FOR_TARGET="$(UCLIBC_CFLAGS_GC) $(GCC_CFLAGS_FOR_TARGET)" \
		CXXFLAGS_FOR_TARGET="$(UCLIBC_CXXFLAGS_GC) $(GCC_CFLAGS_FOR_TARGET)" \
		LDFLAGS_FOR_TARGET="$(UCLIBC_LDFLAGS_GC) -lgcc" \
		install
	$(CP) -Pp '$(PREFIX)/$(UCLIBC_TARGET)/lib/lib'* '$(TARGET_ROOT)/lib'
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_installed

$(FILELIST_DIR)/$(GCC_FILE_LIST): $(STAGEFILES_DIR)/.gcc_installed
	$(TOUCH) $(FILELIST_DIR)/$(GCC_FILE_LIST)

.PHONY: clean-gcc distclean-gcc

clean-gcc:
	-$(RM) -rf $(GCC_BUILD_DIR)
	-$(RM) -rf $(GCC_STAGE1_PREFIX)
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
