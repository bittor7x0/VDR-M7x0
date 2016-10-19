# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 VDR-NG-EM Project
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

GCC_IS_SNAPSHOT := n

ifeq ($(GCC_IS_SNAPSHOT),y)
  GCC_VERSION := 7-20161016
  GCC_FILE := gcc-$(GCC_VERSION).tar.bz2
  GCC_URL := http://www.mirrorservice.org/sites/sourceware.org/pub/gcc/snapshots/$(GCC_VERSION)/$(GCC_FILE)
  GCC_PATCHES_DIR := $(PATCHES_DIR)/gcc/snapshot
  GCC_FILE_LIST += gcc-snapshot.lst
else
  GCC_VERSION := 6.2.0
  GCC_FILE := gcc-$(GCC_VERSION).tar.bz2
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
	$(BZCAT) $(GCC_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
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
                                          $$(PPL_HOSTINSTALLED) \
                                          $$(CLOOG_HOSTINSTALLED) \
                                          $$(UCLIBC_PRE_ALL_GCC)
	-$(RM) -rf $(GCC_STAGE1_BUILD_DIR)
	$(MKDIR) -p $(GCC_STAGE1_BUILD_DIR)
	($(CD) $(GCC_STAGE1_BUILD_DIR) ; \
		export libgcc_cv_fixed_point=no ; \
		export glibcxx_cv_c99_math_tr1=no ; \
		PATH='$(PREFIX_BIN):$(PATH)' \
		LDFLAGS='-lm $(LDFLAGS)' \
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
			--disable-__cxa_atexit \
			--disable-shared \
			--disable-threads \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--with-isl=$(PREFIX) \
			--with-ppl=$(PREFIX) \
			--with-cloog=$(PREFIX) \
			--enable-cloog-backend=isl \
			--disable-ppl-version-check \
			--disable-cloog-version-check \
			--disable-isl-version-check \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libmudflap \
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
                                   $$(LIBELF_HOSTINSTALLED) \
                                   $$(ISL_HOSTINSTALLED) \
                                   $$(PPL_HOSTINSTALLED) \
                                   $$(CLOOG_HOSTINSTALLED) \
                                   $$(UCLIBC_INSTALLED)
	-$(RM) -rf $(GCC_BUILD_DIR)
	$(MKDIR) -p $(GCC_BUILD_DIR)
	($(CD) $(GCC_BUILD_DIR) ; \
		export libgcc_cv_fixed_point=no ; \
		export glibcxx_cv_c99_math_tr1=no ; \
		PATH='$(PREFIX_BIN):$(PATH)' \
		LDFLAGS='-lm $(LDFLAGS)' \
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
			--enable-lto \
			--with-arch=mips2 \
			--with-tune=vr4120 \
			--with-float=soft \
			--with-gmp=$(PREFIX) \
			--with-mpc=$(PREFIX) \
			--with-mpfr=$(PREFIX) \
			--with-libelf=$(PREFIX) \
			--with-isl=$(PREFIX) \
			--with-ppl=$(PREFIX) \
			--with-cloog=$(PREFIX) \
			--enable-cloog-backend=isl \
			--with-host-libstdcxx='-lstdc++' \
			--disable-ppl-version-check \
			--disable-cloog-version-check \
			--disable-isl-version-check \
			--disable-decimal-float \
			--disable-libgomp \
			--disable-libmudflap \
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
		CFLAGS_FOR_TARGET="$(UCLIBC_CFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" \
		CXXFLAGS_FOR_TARGET="$(UCLIBC_CXXFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" \
		LDFLAGS_FOR_TARGET="$(UCLIBC_LDFLAGS_GC) -lgcc" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.gcc_compiled

#
# install gcc
#

$(STAGEFILES_DIR)/.gcc_installed: $(STAGEFILES_DIR)/.gcc_compiled
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GCC_BUILD_DIR) \
		CFLAGS_FOR_TARGET="$(UCLIBC_CFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" \
		CXXFLAGS_FOR_TARGET="$(UCLIBC_CXXFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" \
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
