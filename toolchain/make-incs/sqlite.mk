# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
SQLITE_DEPS = $(BASE_BUILD_STAGEFILE)

SQLITE_VERSION := 3450000
SQLITE_PATCHES_DIR := $(PATCHES_DIR)/sqlite/$(SQLITE_VERSION)

SQLITE_FILE := sqlite-autoconf-$(SQLITE_VERSION).tar.gz
SQLITE_DLFILE := $(DOWNLOAD_DIR)/$(SQLITE_FILE)
SQLITE_URL := https://www.sqlite.org/2024/$(SQLITE_FILE)
SQLITE_DIR := $(BUILD_DIR)/sqlite-autoconf-$(SQLITE_VERSION)
SQLITE_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/sqlite-autoconf-$(SQLITE_VERSION)
SQLITE_CFLAGS_SIZE := $(filter-out -mno-shared,$(UCLIBC_CFLAGS_SIZE)) -flto=auto -fdevirtualize-at-ltrans -ffunction-sections -fdata-sections
# https://www.sqlite.org/footprint.html
SQLITE_CFLAGS := -fno-fast-math -fno-exceptions \
                 -DNDEBUG=1 -DSQLITE_DISABLE_LFS=1 -DSQLITE_OMIT_PROGRESS_CALLBACK=1 \
                 -DSQLITE_OMIT_BUILTIN_TEST=1 -DSQLITE_OMIT_DEPRECATED=1 -DSQLITE_OMIT_UTF16=1 \
                 -DSQLITE_OMIT_EXPLAIN=1 -DSQLITE_OMIT_FOREIGN_KEY=1 -DSQLITE_OMIT_LOAD_EXTENSION=1 \
                 -DSQLITE_OMIT_TCL_VARIABLE=1 -DSQLITE_OMIT_TRACE=1  -DSQLITE_OMIT_DECLTYPE=1 \
                 -DSQLITE_ENABLE_GEOPOLY=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 \
                 -DSQLITE_USE_ALLOCA=1 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1 -DSQLITE_MAX_EXPR_DEPTH=0 \
                 -DSQLITE_OMIT_AUTOINCREMENT=1 -DSQLITE_OMIT_CAST=1 -DSQLITE_OMIT_TEMPDB=1
                 -DSQLITE_OMIT_COMPOUND_SELECT=1 -DSQLITE_OMIT_LOCALTIME=1 \
                 -DSQLITE_OMIT_SCHEMA_VERSION_PRAGMAS=1 -DSQLITE_OMIT_SUBQUERY=1 \
                 -DSQLITE_DISABLE_DIRSYNC=1 -DSQLITE_OMIT_VACUUM=1 -DSQLITE_OMIT_DESERIALIZE=1 -DSQLITE_OMIT_JSON=1

SQLITE_INSTALLED = $(STAGEFILES_DIR)/.sqlite_host_installed $(STAGEFILES_DIR)/.sqlite_installed

PACKS_RULES_$(CONFIG_SQLITE) += $(SQLITE_INSTALLED)
FILE_LISTS_$(CONFIG_SQLITE) += sqlite.lst

CLEAN_RULES += clean-sqlite
DISTCLEAN_RULES += distclean-sqlite

#
# download sqlite
#

$(SQLITE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SQLITE_DLFILE) ] ; then \
	$(WGET) $(SQLITE_URL) -O $(SQLITE_DLFILE) ; \
	fi );
	$(TOUCH) $(SQLITE_DLFILE)

#
# unpack sqlite
#

$(STAGEFILES_DIR)/.sqlite_unpacked: $(SQLITE_DLFILE) \
                                           $(wildcard $(SQLITE_PATCHES_DIR)/*.patch) \
                                           $$(SQLITE_DEPS)
	-$(RM) -rf $(SQLITE_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SQLITE_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_unpacked

#
# patch sqlite
#

$(STAGEFILES_DIR)/.sqlite_patched: $(STAGEFILES_DIR)/.sqlite_unpacked
	$(call patch_package, $(SQLITE_DIR), $(SQLITE_PATCHES_DIR))
	-$(RM) -rf $(SQLITE_HOSTDIR)
	$(CP) -RPp $(SQLITE_DIR) $(HOSTUTILS_BUILD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_patched

#
# configure sqlite in host
#

$(STAGEFILES_DIR)/.sqlite_host_configured: $(STAGEFILES_DIR)/.sqlite_patched
	($(CD) $(SQLITE_HOSTDIR) ; \
		$(SQLITE_HOSTDIR)/configure \
			CFLAGS="$(CFLAGS) $(SQLITE_CFLAGS)" \
			--prefix=$(HOSTUTILS_PREFIX) \
			--disable-shared \
			--enable-static \
			--with-gnu-ld \
			--disable-readline \
			--disable-editline \
			--disable-largefile \
			--disable-fts3 \
			--disable-fts4 \
			--disable-fts5 \
			--disable-rtree \
			--enable-threadsafe \
			--disable-dynamic-extensions)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_host_configured

#
# compile sqlite in host
#

$(STAGEFILES_DIR)/.sqlite_host_compiled: $(STAGEFILES_DIR)/.sqlite_host_configured
	$(MAKE) -C $(SQLITE_HOSTDIR) \
		CFLAGS="$(CFLAGS) $(SQLITE_CFLAGS)" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_host_compiled

#
# install sqlite in host
#

$(STAGEFILES_DIR)/.sqlite_host_installed: $(STAGEFILES_DIR)/.sqlite_host_compiled
	$(MAKE) -C $(SQLITE_HOSTDIR) \
		CFLAGS="$(CFLAGS) $(SQLITE_CFLAGS)" \
		install
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_host_installed

#
# configure sqlite
#

$(STAGEFILES_DIR)/.sqlite_configured: $(STAGEFILES_DIR)/.sqlite_patched
	($(CD) $(SQLITE_DIR) ; \
		$(UCLIBC_ENV_SIZE_LTO_GC) \
		CFLAGS="$(SQLITE_CFLAGS_SIZE) $(SQLITE_CFLAGS)" \
		LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto=auto -fdevirtualize-at-ltrans -fuse-linker-plugin -Wl,--gc-sections" \
		$(SQLITE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static \
			--with-gnu-ld \
			--disable-readline \
			--disable-editline \
			--disable-largefile \
			--disable-fts3 \
			--disable-fts4 \
			--disable-fts5 \
			--disable-rtree \
			--enable-threadsafe \
			--disable-dynamic-extensions)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_configured

#
# compile sqlite
#

$(STAGEFILES_DIR)/.sqlite_compiled: $(STAGEFILES_DIR)/.sqlite_configured
	$(MAKE) -C $(SQLITE_DIR) \
		$(UCLIBC_ENV_SIZE_LTO_GC) \
		CFLAGS="$(SQLITE_CFLAGS_SIZE) $(SQLITE_CFLAGS)" \
		LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto=auto -fdevirtualize-at-ltrans -fuse-linker-plugin -Wl,--gc-sections" \
		all
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_compiled

#
# install sqlite
#

$(STAGEFILES_DIR)/.sqlite_installed: $(STAGEFILES_DIR)/.sqlite_compiled
	$(MAKE) -C $(SQLITE_DIR) \
		$(UCLIBC_ENV_SIZE_LTO_GC) \
		CFLAGS="$(SQLITE_CFLAGS_SIZE) $(SQLITE_CFLAGS)" \
		LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto=auto -fdevirtualize-at-ltrans -fuse-linker-plugin -Wl,--gc-sections" \
		install
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_installed


$(FILELIST_DIR)/sqlite.lst: $(STAGEFILES_DIR)/.sqlite_host_installed \
				$(STAGEFILES_DIR)/.sqlite_installed
	$(TOUCH) $(FILELIST_DIR)/sqlite.lst

.PHONY: clean-sqlite distclean-sqlite

clean-sqlite-host:
	-$(RM) -rf $(SQLITE_HOSTDIR)

clean-sqlite:
	-$(RM) -rf $(SQLITE_DIR)

#
# clean everthing else
#

distclean-sqlite-host:
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_patched
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_host_configured
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_host_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SQLITE_DLFILE)
endif

distclean-sqlite:
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_patched
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_configured
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SQLITE_DLFILE)
endif
