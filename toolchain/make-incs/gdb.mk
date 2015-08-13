# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 VDR-NG-EM Project
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

GDB_DEPS = $(BASE_BUILD_STAGEFILE)

GDB_VERSION := 7.8.2
GDB_PATCHES_DIR := $(PATCHES_DIR)/gdb/$(GDB_VERSION)

GDB_FILE := gdb-$(GDB_VERSION).tar.xz
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
	$(TAR) -C $(BUILD_DIR) -xJf $(GDB_DLFILE)
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
