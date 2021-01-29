# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2021 VDR-NG-EM Project
#
# More information can be found in the file COPYING.
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

MAKE_MAJOR = $(word 1, $(subst ., ,$(MAKE_VERSION)))
MAKE_MINOR = $(word 2, $(subst beta, ,$(subst ., ,$(MAKE_VERSION))))

ifneq ($(shell test \( $(MAKE_MAJOR) -gt 3 \) -o \
                        \( \( $(MAKE_MAJOR) -eq 3 \) \
                        -a \( $(MAKE_MINOR) -ge 81 \) \) && echo ok),ok)
$(error at least make version 3.81 needed)
endif


TOP_DIR := $(abspath .)
HOST_CYGWIN := $(if $(filter Cygwin,$(shell uname -o)),y,n)
HOST_BS := $(shell uname)
HOST_ARCH := $(shell uname -m)
HOST_TARGET := $(shell gcc -dumpmachine)
TARGET := mips-linux
BZCAT := bzcat
AWK := awk
TAR := tar -x
MKDIR := mkdir
RMDIR := rmdir
CP := cp
TOUCH := touch
CD := cd
SED := sed
LN := ln
ECHO := echo
SECHO := @echo
WGET := wget --passive-ftp --no-check-certificate
RM := rm
CAT := cat
CUT := cut
SVN := svn
GIT := git
MV := mv
FIND := find
TEST := test
STAT := stat
GREP := grep
UNZIP := unzip
DD := dd
GZIP := gzip
DATE := date
BASH := bash
MD5 := $(firstword $(wildcard $(addsuffix /md5, $(subst :, ,$(PATH)))) \
                   $(wildcard $(addsuffix /md5sum, $(subst :, ,$(PATH)))))
SHA1 := $(firstword $(wildcard $(addsuffix /sha1, $(subst :, ,$(PATH)))) \
                   $(wildcard $(addsuffix /sha1sum, $(subst :, ,$(PATH)))))
PATCH := patch
SORT := sort

svn_changed = $(shell $(SVN) up $(1) | $(GREP) -c \
                       "^[[:space:]]*[A,D,U,C,G][[:space:]]")

DOWNLOAD_DIR := $(TOP_DIR)/downloads
SCRIPTS_DIR := $(TOP_DIR)/scripts
PATCHES_DIR := $(TOP_DIR)/patches
CONFIGS_DIR := $(TOP_DIR)/configs
PRG_CONFIGS_DIR := $(TOP_DIR)/prg-fw-configs
BUILDIN_DIR := $(TOP_DIR)/buildin
MAKE_INC_DIR := $(TOP_DIR)/make-incs
FILELIST_DIR := $(TOP_DIR)/rootfs-file-lists
HOSTUTILS_MAKE_INC_DIR := $(TOP_DIR)/hostutils-make-incs

include .config

UCLIBC_TARGET := $(TARGET)-uclibc
UCLIBC++_TARGET := $(TARGET)-uclibc++
ifeq ($(strip $(CONFIG_PREFIX)),)
  PREFIX = $(TOP_DIR)/m7x0_xtc_uclibc_$(HOST_BS)-$(HOST_ARCH)
else
  PREFIX := $(abspath $(CONFIG_PREFIX) )
endif

BUILD_DIR := $(TOP_DIR)/build
PREFIX_BIN := $(PREFIX)/bin
TARGET_ROOT := $(PREFIX)/target_root
STAGEFILES_DIR := $(PREFIX)/stagefiles
HOSTUTILS_BUILD_DIR := $(TOP_DIR)/hostutils.build
HOSTUTILS_PREFIX := $(PREFIX)
HOSTUTILS_PREFIX_BIN := $(HOSTUTILS_PREFIX)/bin

LIBGCC_S=$(if $(wildcard $(TARGET_ROOT)/lib/libgcc_s.so),-L$(TARGET_ROOT)/lib -lgcc_s,$(wildcard $(GCC_BUILD_DIR)/$(UCLIBC_TARGET)/libgcc/libgcc.a))

ifneq ($(CONFIG_CCACHE),)
  export CCACHE_DIR=$(STAGEFILES_DIR)/.ccache
  UCLIBC_PATH := $(PREFIX_BIN)/ccache:$(PREFIX_BIN):$(PATH)
else
  UCLIBC_PATH := $(PREFIX_BIN):$(PATH)
endif

UCLIBC_CC := $(UCLIBC_TARGET)-gcc
UCLIBC_CXX := $(UCLIBC_TARGET)-g++
UCLIBC++_CXX := $(UCLIBC++_TARGET)-g++
UCLIBC++_CXX_STD := $(UCLIBC++_CXX)-std
UCLIBC_AS := $(UCLIBC_TARGET)-as
UCLIBC_AR := $(UCLIBC_TARGET)-ar
UCLIBC_CPP := $(UCLIBC_TARGET)-cpp
UCLIBC_RANLIB := $(UCLIBC_TARGET)-ranlib
UCLIBC_LD := $(UCLIBC_TARGET)-ld
UCLIBC_NM := $(UCLIBC_TARGET)-nm
UCLIBC_STRIP := sstrip
UCLIBC_OBJCOPY := $(UCLIBC_TARGET)-objcopy

# Flags for optimize loops
UCLIBC_CFLAGS_LOOPS := -funroll-loops -floop-interchange -floop-strip-mine -floop-block -ftree-loop-linear

# Standard flags for any program
UCLIBC_CFLAGS := -DNDEBUG -march=mips2 -mtune=vr4120 -mfix-vr4120 -Ofast -msoft-float
UCLIBC_CXXFLAGS := $(UCLIBC_CFLAGS) -fvisibility-inlines-hidden
UCLIBC_LDFLAGS := -Wl,-O1 -Wl,--as-needed -Wl,--relax -Wl,--sort-common -Wl,--sort-section,alignment

UCLIBC_ENV = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS)" CXXFLAGS="$(UCLIBC_CXXFLAGS)" LDFLAGS="$(UCLIBC_LDFLAGS)"

UCLIBC_ENV_LOOPS = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS) $(UCLIBC_CFLAGS_LOOPS)" CXXFLAGS="$(UCLIBC_CXXFLAGS) $(UCLIBC_CFLAGS_LOOPS)" LDFLAGS="$(UCLIBC_LDFLAGS)"

# Flags for remove unused sections
UCLIBC_CFLAGS_GC := $(UCLIBC_CFLAGS) -ffunction-sections -fdata-sections
UCLIBC_CXXFLAGS_GC := $(UCLIBC_CXXFLAGS) -ffunction-sections -fdata-sections
UCLIBC_LDFLAGS_GC := $(UCLIBC_LDFLAGS) -Wl,--gc-sections

UCLIBC_ENV_GC = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_GC)" CXXFLAGS="$(UCLIBC_CXXFLAGS_GC)" LDFLAGS="$(UCLIBC_LDFLAGS_GC)"

UCLIBC_ENV_GC_LOOPS = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" CXXFLAGS="$(UCLIBC_CXXFLAGS_GC) $(UCLIBC_CFLAGS_LOOPS)" LDFLAGS="$(UCLIBC_LDFLAGS_GC)"

# Flags for use Link Time Optimization
UCLIBC_CFLAGS_LTO := $(UCLIBC_CFLAGS) -flto
UCLIBC_CXXFLAGS_LTO := $(UCLIBC_CXXFLAGS) -flto
UCLIBC_LDFLAGS_LTO := $(UCLIBC_LDFLAGS) -flto -fwhole-program

UCLIBC_ENV_LTO = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_LTO)" CXXFLAGS="$(UCLIBC_CXXFLAGS_LTO)" LDFLAGS="$(UCLIBC_LDFLAGS_LTO)"

UCLIBC_ENV_LTO_LOOPS = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_LTO) $(UCLIBC_CFLAGS_LOOPS)" CXXFLAGS="$(UCLIBC_CXXFLAGS_LTO) $(UCLIBC_CFLAGS_LOOPS)" LDFLAGS="$(UCLIBC_LDFLAGS_LTO)"

# Flags for use Link Time Optimization and remove unused sections
UCLIBC_CFLAGS_LTO_GC := $(UCLIBC_CFLAGS) -flto -ffunction-sections -fdata-sections
UCLIBC_CXXFLAGS_LTO_GC := $(UCLIBC_CXXFLAGS) -flto -ffunction-sections -fdata-sections
UCLIBC_LDFLAGS_LTO_GC := $(UCLIBC_LDFLAGS) -flto -fwhole-program -Wl,--gc-sections

UCLIBC_ENV_LTO_GC = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_LTO_GC)" CXXFLAGS="$(UCLIBC_CXXFLAGS_LTO_GC)" LDFLAGS="$(UCLIBC_LDFLAGS_LTO_GC)"

UCLIBC_ENV_LTO_GC_LOOPS = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_LTO_GC) $(UCLIBC_CFLAGS_LOOPS)" CXXFLAGS="$(UCLIBC_CXXFLAGS_LTO_GC) $(UCLIBC_CFLAGS_LOOPS)" LDFLAGS="$(UCLIBC_LDFLAGS_LTO_GC)"

# Flags for shrink size
UCLIBC_CFLAGS_SIZE := $(filter-out -Ofast,$(UCLIBC_CFLAGS_GC)) -Os -mplt -mno-shared -mabicalls -fno-reorder-blocks -fno-tree-ch -fno-caller-saves -fno-inline-small-functions -fno-reorder-functions
UCLIBC_CXXFLAGS_SIZE := $(UCLIBC_CFLAGS_SIZE) -fvisibility-inlines-hidden
UCLIBC_LDFLAGS_SIZE := $(UCLIBC_LDFLAGS_GC)

UCLIBC_ENV_SIZE = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_SIZE)" CXXFLAGS="$(UCLIBC_CXXFLAGS_SIZE)" LDFLAGS="$(UCLIBC_LDFLAGS_SIZE)"

UCLIBC_ENV_SIZE_LTO = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_SIZE) -flto" CXXFLAGS="$(UCLIBC_CXXFLAGS_SIZE) -flto" LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto -fwhole-program"

UCLIBC_ENV_SIZE_LOOPS = PATH="$(UCLIBC_PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" NM="$(UCLIBC_NM)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS_SIZE) $(UCLIBC_CFLAGS_LOOPS)" CXXFLAGS="$(UCLIBC_CXXFLAGS_SIZE) $(UCLIBC_CFLAGS_LOOPS)" LDFLAGS="$(UCLIBC_LDFLAGS_SIZE)"

.SECONDEXPANSION:
.PHONY: all

all: $(STAGEFILES_DIR)/.toolchain-ready ;

define no_config_errormsg
*
Configuration needed.
You need to edit .config by hand or copy configs/default-config-pro or configs/default-config-lite to .config
Interactive generation of .config not yet implemented.
*
endef
.config:
	$(error $(no_config_errormsg))

ifeq ($(CONFIG_M7X0_TYPE),m740)
   CRAMFS_MAX_IMGSIZE := 6291456
   M7X0_KERNEL_DIR := m740_kernel
else ifeq ($(CONFIG_M7X0_TYPE),m750s)
   CRAMFS_MAX_IMGSIZE := 8388608
   M7X0_KERNEL_DIR := m750s_kernel
else
   $(error unknown system type given)
endif
KERNEL_MAX_IMGSIZE := 786432
ROOTFS_MAX_IMGSIZE := $(CRAMFS_MAX_IMGSIZE)

gen_copy_lst_dir = \
  $(ECHO) "$(1)       -                 d 755 0 0 - - - - -"; \
  $(eval gen_copy_lst_dir_accu := $(1)/)
gen_copy_lst_dirs = \
   $(eval gen_copy_lst_dir_accu := ) \
   $(foreach dir,$(strip $(subst /, ,$(1))), \
      $(call gen_copy_lst_dir,$(gen_copy_lst_dir_accu)$(dir)))

# functions for generate in copy files
# should be called
# $(call gen_copy_file_lst,<path_of_files>,<path in root fs>,<user id>,<group id>,<lst filename>,check|nocheck)
define gen_copy_file_lst
$(FILELIST_DIR)/$(5): $(shell if [ -d "$(1)" ] ; then $(FIND) $(1) \! -path "*.svn*" \! -name "*~" ; else [ "$(6)" != "nocheck" ] && echo $(1) ; fi)
	if [ -d "$(1)" ] ; then \
	( $(call gen_copy_lst_dirs,$(2)) \
		$(FIND) "$(1)" -type d \! -path "$(1)" \! -path "*.svn*" \
			-printf "$(2)/%-20P - d %m $(3) $(4) - - - - -\n" ; \
		$(FIND) "$(1)" -type f \! -path "*.svn*" \! -name "*~" \
			-printf "$(2)/%-20P %-20p f %m $(3) $(4) - - - - -\n" ; \
		$(FIND) $(1) -type l -printf "$(2)/%-20P %-20l l %m $(3) $(4) - - - - -\n" ; ) \
	> $(FILELIST_DIR)/$(5) ; \
	fi
	$(TOUCH) $(FILELIST_DIR)/$(5)
endef

# patches a package from a given patch-dir. all *.patch files are patched with patch -p1
# usage: $(call patch_package, <dest dir>, <patchdir>)
define patch_package
	for patch_file in `$(FIND) $(2) -type f -iname '*.patch' | $(SORT)` ; do \
		$(PATCH) -p1 -E -d $(1) < $$patch_file ; \
	done
endef

# optimizes CSS code from a given css-dir. all *.css files are reduced with YUI Compressor
# usage: $(call css_shrink_dir, <css_dir>)
define css_shrink_dir
	for css_file in `$(FIND) $(1) -type f -iname '*.css' | $(SORT)` ; do \
		$(SED) -i -e  "s,/\*\!,/\*,g" "$$css_file" ; \
		$(shell which java) -jar $(YUI_COMPRESSOR_JAR) --type css "$$css_file" -o "$$css_file" ; \
	done
endef

# optimizes javascript code from a given js-dir. all *.js files are reduced with YUI Compressor
# usage: $(call js_shrink_dir, <js_dir>)
define js_shrink_dir
	for js_file in `$(FIND) $(1) -type f -iname '*.js' | $(SORT)` ; do \
		$(SED) -i -e  "s,/\*\!,/\*,g" "$$js_file" ; \
		$(shell which java) -jar $(YUI_COMPRESSOR_JAR) --type js "$$js_file" -o "$$js_file" ; \
	done
endef

# optimizes png images from a given png-dir. all *.png files are reduced with multiple tools
# usage: $(call png_shrink_dir, <png_dir>)
define png_shrink_dir
	for png_file in `$(FIND) $(1) -type f -iname '*.png' | $(SORT)` ; do \
		if [ $(CONFIG_PINGO) == y ] ; then \
			$(ECHO) "--- pingo ---" ; \
			($(CD) "$$(dirname $$png_file)"; $(PINGO_BIN) -sb "$$(basename $$png_file)") ; \
		fi; \
		if [ $(CONFIG_ECT) == y ] ; then \
			$(ECHO) "--- Efficient Compression Tool ---" ; \
			$(ECT_BIN) --allfilters-b -strip -9 "$$png_file" ; \
		fi; \
		if [ $(CONFIG_PNGWOLF-ZOPFLI) == y ] ; then \
			$(ECHO) "--- pngwolf-zopfli ---" ; \
			$(PNGWOLF-ZOPFLI_BIN) --out-deflate=zopfli,iter=30 --in="$$png_file" --out="$$png_file" ; \
		fi; \
		if [ $(CONFIG_ZOPFLIPNG) == y ] ; then \
			$(ECHO) "--- ZopfliPNG ---" ; \
			$(ZOPFLIPNG_BIN) -m -y --lossy_transparent --filters=0me "$$png_file" "$$png_file" ; \
		fi; \
		if [ $(CONFIG_PNGOUT) == y ] ; then \
			$(ECHO) "--- PNGOUT ---" ; \
			$(PNGOUT_BIN) "$$png_file" -s0 -k0 -y ; \
		fi; \
	done
endef

# used in set_lang
ifeq ($(CONFIG_LANGUAGE),es)
   CONFIG_LANGUAGE_SETUP := esl
   CONFIG_LANGUAGE_LC := es_ES
   CONFIG_LANGUAGE_OSD := 10
   CONFIG_LANGUAGE_WEB := 2
else ifeq ($(CONFIG_LANGUAGE),en)
   CONFIG_LANGUAGE_SETUP := eng
   CONFIG_LANGUAGE_LC := en_US
   CONFIG_LANGUAGE_OSD := 0
   CONFIG_LANGUAGE_WEB := 0
else ifeq ($(CONFIG_LANGUAGE),de)
   CONFIG_LANGUAGE_SETUP := deu
   CONFIG_LANGUAGE_LC := de_DE
   CONFIG_LANGUAGE_OSD := 1
   CONFIG_LANGUAGE_WEB := 1
else ifeq ($(CONFIG_LANGUAGE),fr)
   CONFIG_LANGUAGE_SETUP := fra
   CONFIG_LANGUAGE_LC := fr_FR
   CONFIG_LANGUAGE_OSD := 6
   CONFIG_LANGUAGE_WEB := 3
else
   $(error unknown language given)
endif

# set language (CONFIG_LANGUAGE) in any files from a given dir.
# usage: $(call set_lang, <path_of_files>)
define set_lang
	(if [ -f $(1)/etc/vdr/setup.conf ] ; then \
		$(SED) -i -e "s/^AudioLanguages.*/AudioLanguages = $(CONFIG_LANGUAGE_SETUP)/" $(1)/etc/vdr/setup.conf ; \
		$(SED) -i -e "s/^EPGLanguages.*/EPGLanguages = $(CONFIG_LANGUAGE_SETUP)/" $(1)/etc/vdr/setup.conf ; \
		$(SED) -i -e "s/^OSDLanguage.*/OSDLanguage = $(CONFIG_LANGUAGE_OSD)/" $(1)/etc/vdr/setup.conf ; \
	fi); \
	(if [ -f $(1)/etc/vdr/runvdr ] ; then \
		$(SED) -i -e "s/^export LC_ALL.*/export LC_ALL=$(CONFIG_LANGUAGE_LC)/" $(1)/etc/vdr/runvdr ; \
	fi ); \
	(if [ -f $(1)/etc/webif/webif.conf ] ; then \
		$(SED) -i -e "s/^default language.*/default language = $(CONFIG_LANGUAGE_WEB)/" $(1)/etc/webif/webif.conf ; \
	fi); \
	(if [ -f $(1)/etc/vdr/plugins/epgsearch/epgsearchconflmail.templ.eng -a -f $(1)/etc/vdr/plugins/epgsearch/epgsearchupdmail.templ.eng ] ; then \
		if [ $(CONFIG_LANGUAGE) == es ] ; then \
			$(RM) -f $(1)/etc/vdr/plugins/epgsearch/epgsearchconflmail.templ.eng ; \
			$(RM) -f $(1)/etc/vdr/plugins/epgsearch/epgsearchupdmail.templ.eng ; \
		else \
			$(MV) -f $(1)/etc/vdr/plugins/epgsearch/epgsearchconflmail.templ.eng $(1)/etc/vdr/plugins/epgsearch/epgsearchconflmail.templ ; \
			$(MV) -f $(1)/etc/vdr/plugins/epgsearch/epgsearchupdmail.templ.eng $(1)/etc/vdr/plugins/epgsearch/epgsearchupdmail.templ ; \
		fi; \
	fi);
endef

BASE_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.base_build
PACKS_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.packs_build
POST_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.post_build

TC_INIT_RULE = $(STAGEFILES_DIR)/.uclibctoolchaindirs

BASE_RULES_y = 
BASE_RULES_n =
BASE_RULES_ =
BASE_RULES = $(BASE_RULES_y)

PACKS_RULES_y =
PACKS_RULES_n =
PACKS_RULES_ =
PACKS_RULES = $(PACKS_RULES_y)

POST_RULES_y =
POST_RULES_n =
POST_RULES_ =
POST_RULES = $(POST_RULES_y)

CLEAN_RULES =
DISTCLEAN_RULES =

# base-common and base-<lite|pro> are alway needed
FILE_LISTS_y = base-common.lst base-$(CONFIG_FW_VERSION).lst
FILE_LISTS_n =
FILE_LISTS_ =
FILE_LISTS = $(FILE_LISTS_y)

MAKE_SUBS := $(sort $(wildcard $(HOSTUTILS_MAKE_INC_DIR)/*.mk)) $(wildcard $(MAKE_INC_DIR)/*.mk)
include $(MAKE_SUBS)

GUI-TOOL-UTILS := $(STAGEFILES_DIR)/.base_build \
  $(HOSTUTILS_PREFIX_BIN)/cramfsck \
  $(HOSTUTILS_PREFIX_BIN)/mkcramfs $(HOSTUTILS_PREFIX_BIN)/cramfsswap \
  $(STAGEFILES_DIR)/.fw-utils_host_installed \
  $(STAGEFILES_DIR)/.rsacoder_host_installed \
  $(STAGEFILES_DIR)/.binutils_installed

.PHONY: clean distclean

$(STAGEFILES_DIR)/.uclibctoolchaindirs:
	-$(RM) -rf $(STAGEFILES_DIR)
	-$(RM) -rf $(BUILD_DIR)
	-$(RM) -rf $(HOSTUTILS_BUILD_DIR)
ifeq ($(strip $(CONFIG_PREFIX)),)
	-$(RM) -rf $(PREFIX)
	-$(RM) -rf $(HOSTUTILS_PREFIX)
endif
	-$(MKDIR) -p $(BUILD_DIR)
	-$(MKDIR) -p $(PREFIX)
	-$(MKDIR) -p $(TARGET_ROOT)
	-$(MKDIR) -p $(TARGET_ROOT)/bin
	-$(MKDIR) -p $(TARGET_ROOT)/etc
	-$(MKDIR) -p $(TARGET_ROOT)/lib
	-$(MKDIR) -p $(TARGET_ROOT)/sbin
	-$(MKDIR) -p $(TARGET_ROOT)/usr
	-$(MKDIR) -p $(TARGET_ROOT)/usr/bin
	-$(MKDIR) -p $(TARGET_ROOT)/usr/lib
	-$(MKDIR) -p $(TARGET_ROOT)/usr/sbin
	-$(MKDIR) -p $(STAGEFILES_DIR)
	-$(MKDIR) -p $(HOSTUTILS_BUILD_DIR)
	-$(MKDIR) -p $(HOSTUTILS_PREFIX_BIN)
	$(TOUCH) $(STAGEFILES_DIR)/.uclibctoolchaindirs

$(BASE_BUILD_STAGEFILE): $(BASE_RULES)
	$(TOUCH) $(BASE_BUILD_STAGEFILE)

$(PACKS_BUILD_STAGEFILE): $(PACKS_RULES)
	$(TOUCH) $(PACKS_BUILD_STAGEFILE)

$(POST_BUILD_STAGEFILE): $(POST_RULES)
	$(TOUCH) $(POST_BUILD_STAGEFILE)

$(STAGEFILES_DIR)/.toolchain-ready: $(BASE_BUILD_STAGEFILE) \
                                    $(PACKS_BUILD_STAGEFILE) \
                                    $(POST_BUILD_STAGEFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.toolchain-ready
	$(SECHO)
	$(SECHO) Finished, we are ready to rumble...
	$(SECHO)

gui-tool: $(GUI-TOOL-UTILS)
	$(SECHO) Finished, build tools for a gui tool

clean: $(CLEAN_RULES)
	-$(RM) -rf $(BUILD_DIR)
	-$(RM) -rf $(HOSTUTILS_BUILD_DIR)

distclean: clean $(DISTCLEAN_RULES)
ifeq ($(strip $(CONFIG_PREFIX)),)
	-$(RM) -rf $(PREFIX)
	-$(RM) -rf $(HOSTUTILS_PREFIX)
endif
