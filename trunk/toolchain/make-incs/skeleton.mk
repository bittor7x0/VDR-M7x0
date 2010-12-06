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
# $Id: skeleton.mk 401 2009-03-26 12:29:09Z andreas $
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
SKELETON_DEPS = $(BASE_BUILD_STAGEFILE) $(SKELETON1_INSTALLED) $(SKELETON2_INSTALLED)

ifeq ($(CONFIG_SKELETON),y)
# Check dependencies here if needed
ifneq ($(and $(filter y,$(CONFIG_SKELETON1)) $(filter y,$(CONFIG_SKELETON2))),y)
   $(error dependency error: skeleton needs skeleton1 and skeleton2 enabled)
endif
endif

# Version
SKELETON_VERSION := 1.0
# Patches dir
SKELETON_PATCHES_DIR := $(PATCHES_DIR)/skeleton/$(SKELETON_VERSION)

# Compressed file of package
SKELETON_FILE := skeleton-$(SKELETON_VERSION).tar.gz
# Full path of download filed
# all downloads should go to $(DOWNLOAD_DIR)
SKELETON_DLFILE := $(DOWNLOAD_DIR)/$(SKELETON_FILE)
# Download url for package
SKELETON_URL := ftp://ftp.gnu.org/gnu/skeleton/$(SKELETON_FILE)
# Dir package is extracted to
# all dirs need for build should go to $(BUILD_DIR)
SKELETON_DIR := $(BUILD_DIR)/skeleton-$(SKELETON_VERSION)
# If possible use a seperate build dir
SKELETON_BUILD_DIR := $(BUILD_DIR)/skeleton.build

# Stagefile/Rule used by other packages for dependencies
SKELETON_INSTALLED = $(STAGEFILES_DIR)/.skeleton_installed

# If package enabled by config append rule to build an install
# package
PACKS_RULES_$(CONFIG_SKELETON) += $(SKELETON_INSTALLED)
# File list for files going to root fs if package enabled
# for format of file see rootfs-file-lists/skeleton.lst
FILE_LISTS_$(CONFIG_SKELETON) += skeleton.lst

# clean rules
CLEAN_RULES += clean-skeleton
DISTCLEAN_RULES += distclean-skeleton

# all build steps are handled by stagefiles,
# which should be put in $(STAGEFILES_DIR) dir

#
# download package
#

$(SKELETON_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SKELETON_DLFILE) ] ; then \
	$(WGET) $(SKELETON_URL) -O $(SKELETON_DLFILE) ; \
	fi );
	$(TOUCH) $(SKELETON_DLFILE)

#
# unpack package relativ to $(BUILD_DIR)
# should depend on patches to rebuild if patches changed
# as all patch are applied at once on extraced dir
#

$(STAGEFILES_DIR)/.skeleton_unpacked: $(SKELETON_DLFILE) \
                                      $(wildcard $(SKELETON_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(SKELETON_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SKELETON_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.skeleton_unpacked

#
# patch package
# all patch are applied at once on extraced dir
# and should be named *.patch

$(STAGEFILES_DIR)/.skeleton_patched: $(STAGEFILES_DIR)/.skeleton_unpacked
	$(call patch_package, $(SKELETON_DIR), $(SKELETON_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.skeleton_patched

#
# configure package
# if possible in an own build dir.
# dependencies should put here, second expansion is needed.
# all packages should install somewhere below $(TARGET_ROOT)
# and configured for $(TARGET) (or $(UCLIBC_TARGET) if this works)
# $(UCLIBC_ENV) setups some environment variables as CFLAGS, PATH ...

$(STAGEFILES_DIR)/.skeleton_configured: $(STAGEFILES_DIR)/.skeleton_patched \
                                        $$(SKELETON_DEPS)
	-$(RM) -rf $(SKELETON_BUILD_DIR)
	$(MKDIR) -p $(SKELETON_BUILD_DIR)
	($(CD) $(SKELETON_BUILD_DIR) ; $(UCLIBC_ENV) \
		$(SKELETON_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.skeleton_configured

#
# compile package
#

$(STAGEFILES_DIR)/.skeleton_compiled: $(STAGEFILES_DIR)/.skeleton_configured
	$(UCLIBC_ENV) $(MAKE) -C $(SKELETON_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.skeleton_compiled

#
# install package
#

$(STAGEFILES_DIR)/.skeleton_installed: $(STAGEFILES_DIR)/.skeleton_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(SKELETON_BUILD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.skeleton_installed

#
# generate rootfs file list if need.
# file list should depend on installed package
#

$(FILELIST_DIR)/skeleton.lst: $(STAGEFILES_DIR)/.skeleton_installed
	$(TOUCH) $(FILELIST_DIR)/skeleton.lst

.PHONY: clean-skeleton distclean-skeleton

#
# clean package build (build dirs)
# stagefiles should left intact, to support rootfs generation
# without the need of rebuild everthing
#

clean-skeleton:
	-$(RM) -rf $(SKELETON_BUILD_DIR)
	-$(RM) -rf $(SKELETON_DIR)

#
# clean everthing else
#

distclean-skeleton:
	-$(RM) -f $(STAGEFILES_DIR)/.skeleton_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.skeleton_patched
	-$(RM) -f $(STAGEFILES_DIR)/.skeleton_configured
	-$(RM) -f $(STAGEFILES_DIR)/.skeleton_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.skeleton_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SKELETON_DLFILE)
endif
