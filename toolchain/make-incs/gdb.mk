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

GDB_DEPS = $(BASE_BUILD_STAGEFILE)

GDB_VERSION := 6.8
GDB_PATCHES_DIR := $(PATCHES_DIR)/gdb/$(GDB_VERSION)

GDB_FILE := gdb-$(GDB_VERSION)a.tar.bz2
GDB_DLFILE := $(DOWNLOAD_DIR)/$(GDB_FILE)
GDB_DIR := $(BUILD_DIR)/gdb-$(GDB_VERSION)
GDB_BUILD_DIR := $(BUILD_DIR)/gdb.build
GDB_URL := ftp://ftp.gnu.org/gnu/gdb/$(GDB_FILE)

GDB_INSTALLED = $(STAGEFILES_DIR)/.gdb_installed

PACKS_RULES_$(CONFIG_GDB) += $(GDB_INSTALLED)

CLEAN_RULES += clean-gdb
DISTCLEAN_RULES += distclean-gdb

#
# download gdb
#

$(GDB_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(GDB_DLFILE) ] ; then \
	$(WGET) $(GDB_URL) -O $(GDB_DLFILE) ; \
	fi );
	$(TOUCH) $(GDB_DLFILE)


#
# unpack gdb
#

$(STAGEFILES_DIR)/.gdb_unpacked: $(GDB_DLFILE) $(wildcard $(GDB_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(GDB_DIR)
	$(BZCAT) $(GDB_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.gdb_unpacked

#
# patch gdb
#

$(STAGEFILES_DIR)/.gdb_patched: $(STAGEFILES_DIR)/.gdb_unpacked
	$(call patch_package, $(GDB_DIR), $(GDB_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.gdb_patched

#
# configure gdb
#

$(STAGEFILES_DIR)/.gdb_configured: $(STAGEFILES_DIR)/.gdb_patched $$(GDB_DEPS)
	-$(RM) -rf $(GDB_BUILD_DIR)
	$(MKDIR) $(GDB_BUILD_DIR)
	($(CD) $(GDB_BUILD_DIR) ; PATH='$(PREFIX_BIN):$(PATH)' \
		$(GDB_DIR)/configure \
			--prefix=$(PREFIX) \
			--with-sysroot=$(TARGET_ROOT) \
			--disable-werror \
			--target=$(UCLIBC_TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.gdb_configured

#
# compile gdb
#

$(STAGEFILES_DIR)/.gdb_compiled: $(STAGEFILES_DIR)/.gdb_configured
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GDB_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.gdb_compiled

#
# install gdb
#

$(STAGEFILES_DIR)/.gdb_installed: $(STAGEFILES_DIR)/.gdb_compiled
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) -C $(GDB_BUILD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.gdb_installed

.PHONY: clean-gdb distclean-gdb

clean-gdb:
	-$(RM) -rf $(GDB_DIR)
	-$(RM) -rf $(GDB_BUILDDIR)

distclean-gdb:
	-$(RM) -f $(STAGEFILES_DIR)/.gdb_patched
	-$(RM) -f $(STAGEFILES_DIR)/.gdb_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.gdb_configured
	-$(RM) -f $(STAGEFILES_DIR)/.gdb_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.gdb_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(GDB_DLFILE)
endif
