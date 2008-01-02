#
# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
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
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id$
#

ifeq ($(basename $(shell readlink /bin/sh)),dash)
$(error dash as /bin/sh not supported correct your symlink /bin/sh)
endif

MAKE_MAJOR = $(word 1, $(subst ., ,$(MAKE_VERSION)))
MAKE_MINOR = $(word 2, $(subst beta, ,$(subst ., ,$(MAKE_VERSION))))

ifneq ($(shell test \( $(MAKE_MAJOR) -gt 3 \) -o \
                        \( \( $(MAKE_MAJOR) -eq 3 \) \
                        -a \( $(MAKE_MINOR) -ge 81 \) \) && echo ok),ok)
$(error at least make version 3.81 needed)
endif


TOP_DIR := $(abspath .)
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
WGET := wget --passive-ftp
RM := rm
CAT := cat
CUT := cut
SVN := svn
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


UCLIBC_CC := $(UCLIBC_TARGET)-gcc
UCLIBC_CXX := $(UCLIBC_TARGET)-g++
UCLIBC_AS := $(UCLIBC_TARGET)-as
UCLIBC_AR := $(UCLIBC_TARGET)-ar
UCLIBC_CPP := $(UCLIBC_TARGET)-cpp
UCLIBC_RANLIB := $(UCLIBC_TARGET)-ranlib
UCLIBC_LD := $(UCLIBC_TARGET)-ld
UCLIBC_STRIP := $(UCLIBC_TARGET)-strip
UCLIBC_OBJCOPY := $(UCLIBC_TARGET)-objcopy
UCLIBC_CFLAGS := -march=mips2 -mtune=vr4120 -O3 -msoft-float -Wa,-mfix-vr4120
UCLIBC_CXXFLAGS := $(UCLIBC_CFLAGS)

UCLIBC_ENV = PATH="$(PREFIX_BIN):$(PATH)" CC="$(UCLIBC_CC)" \
   CXX="$(UCLIBC_CXX)" AS="$(UCLIBC_AS)" AR="$(UCLIBC_AR)" CPP="$(UCLIBC_CPP)" \
   RANLIB="$(UCLIBC_RANLIB)" LD="$(UCLIBC_LD)" STRIP="$(UCLIBC_STRIP)" \
   CFLAGS="$(UCLIBC_CFLAGS)" CXXFLAGS="$(UCLIBC_CXXFLAGS)"

.SECONDEXPANSION:
.PHONY: all

all: $(STAGEFILES_DIR)/.toolchain-ready ;

define no_config_errormsg
*
Configuration needed.
You need to edit .config by hand or copy configs/default-config to .config
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

gen_copy_lst_dir = \
  $(ECHO) "$(1)       -                 d 755 0 0 - - - - -"; \
  $(eval gen_copy_lst_dir_accu := $(1)/)
gen_copy_lst_dirs = \
   $(eval gen_copy_lst_dir_accu := ) \
   $(foreach dir,$(strip $(subst /, ,$(1))), \
      $(call gen_copy_lst_dir,$(gen_copy_lst_dir_accu)$(dir)))

# functions for generate in copy files
# should be called
# $(call gen_copy_file_lst,<path_of_files>,<path in root fs>,<user id>,<group id>,<lst filename>)
define gen_copy_file_lst
$(FILELIST_DIR)/$(5): $(shell $(FIND) $(1) \! -path "*.svn*" \! -name "*~") ; \
	( $(call gen_copy_lst_dirs,$(2)) \
		$(FIND) $(1) -type d \! -path "$(1)" \! -path "*.svn*" \
			-printf "$(2)/%-20P - d %m $(3) $(4) - - - - -\n" ; \
		$(FIND) $(1) -type f \! -path "*.svn*" \! -name "*~" \
			-printf "$(2)/%-20P %-20p f %m $(3) $(4) - - - - -\n" ; \
		$(FIND) $(1) -type l -printf "$(2)/%-20P %-20l l %m $(3) $(4) - - - - -\n" ; ) \
	> $(FILELIST_DIR)/$(5)

endef

BASE_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.base_build
PACKS_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.packs_build
POST_BUILD_STAGEFILE := $(STAGEFILES_DIR)/.post_build

BASE_RULES_y = $(STAGEFILES_DIR)/.uclibctoolchaindirs
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

# base is alway needed
FILE_LISTS_y = base.lst
FILE_LISTS_n =
FILE_LISTS_ =
FILE_LISTS = $(FILE_LISTS_y)

MAKE_SUBS := $(wildcard $(HOSTUTILS_MAKE_INC_DIR)/*.mk) $(wildcard $(MAKE_INC_DIR)/*.mk)
include $(MAKE_SUBS)

GUI-TOOL-UTILS := $(STAGEFILES_DIR)/.base_build \
  $(HOSTUTILS_PREFIX_BIN)/cramfsck \
  $(HOSTUTILS_PREFIX_BIN)/mkcramfs $(HOSTUTILS_PREFIX_BIN)/cramfsswap \
  $(STAGEFILES_DIR)/.fw-utils_host_installed \
  $(STAGEFILES_DIR)/.rsacoder_host_installed \
  $(STAGEFILES_DIR)/.binutils_installed

.PHONY: clean distclean

$(STAGEFILES_DIR)/.uclibctoolchaindirs:
#	-$(RM) -rf $(STAGEFILES_DIR)
#	-$(RM) -rf $(BUILD_DIR)
	-$(RM) -rf $(HOSTUTILS_BUILD_DIR)
ifeq ($(strip $(CONFIG_PREFIX)),)
#	-$(RM) -rf $(PREFIX)
#	-$(RM) -rf $(HOSTUTILS_PREFIX)
endif
#	-$(MKDIR) -p $(BUILD_DIR)
#	-$(MKDIR) -p $(PREFIX)
	-$(MKDIR) -p $(TARGET_ROOT)
	-$(MKDIR) -p $(TARGET_ROOT)/etc
#	-$(MKDIR) -p $(STAGEFILES_DIR)
	-$(MKDIR) -p $(HOSTUTILS_BUILD_DIR)
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
