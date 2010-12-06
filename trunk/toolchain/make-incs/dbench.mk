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
DBENCH_DEPS = $(BASE_BUILD_STAGEFILE)

DBENCH_VERSION := 3.04
DBENCH_PATCHES_DIR := $(PATCHES_DIR)/dbench/$(DBENCH_VERSION)

DBENCH_FILE := dbench-$(DBENCH_VERSION).tar.gz
DBENCH_DLFILE := $(DOWNLOAD_DIR)/$(DBENCH_FILE)
DBENCH_URL := http://samba.org/ftp/tridge/dbench/$(DBENCH_FILE)
DBENCH_DIR := $(BUILD_DIR)/dbench-$(DBENCH_VERSION)

DBENCH_PACKED = $(STAGEFILES_DIR)/.dbench_packed

PACKS_RULES_$(CONFIG_DBENCH) += $(DBENCH_PACKED)

CLEAN_RULES += clean-dbench
DISTCLEAN_RULES += distclean-dbench

#
# download dbench
#

$(DBENCH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(DBENCH_DLFILE) ] ; then \
	$(WGET) $(DBENCH_URL) -O $(DBENCH_DLFILE) ; \
	fi );
	$(TOUCH) $(DBENCH_DLFILE)

#
# unpack dbench
#

$(STAGEFILES_DIR)/.dbench_unpacked: $(DBENCH_DLFILE) \
                                           $(wildcard $(DBENCH_PATCHES_DIR)/*.patch) \
                                           $$(DBENCH_DEPS)
	-$(RM) -rf $(DBENCH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(DBENCH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.dbench_unpacked

#
# patch dbench
#

$(STAGEFILES_DIR)/.dbench_patched: $(STAGEFILES_DIR)/.dbench_unpacked
	$(call patch_package, $(DBENCH_DIR), $(DBENCH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.dbench_patched

#
# configure dbench
#

$(STAGEFILES_DIR)/.dbench_configured: $(STAGEFILES_DIR)/.dbench_patched
	($(CD) $(DBENCH_DIR) ; $(DBENCH_DIR)/autogen.sh ; \
		$(UCLIBC_ENV) $(DBENCH_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.dbench_configured

#
# compile dbench
#

$(STAGEFILES_DIR)/.dbench_compiled: $(STAGEFILES_DIR)/.dbench_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(DBENCH_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dbench_compiled

#
# pack dbench
#

$(STAGEFILES_DIR)/.dbench_packed: $(STAGEFILES_DIR)/.dbench_compiled
	$(PREFIX_BIN)/$(UCLIBC_STRIP) $(DBENCH_DIR)/dbench
	$(PREFIX_BIN)/$(UCLIBC_STRIP) $(DBENCH_DIR)/tbench
	$(PREFIX_BIN)/$(UCLIBC_STRIP) $(DBENCH_DIR)/tbench_srv
	-$(RM) -f $(TOP_DIR)/dbench-$(DBENCH_VERSION)-m7x0.tar.bz2
	tar jcvf $(TOP_DIR)/dbench-$(DBENCH_VERSION)-m7x0.tar.bz2 \
		--directory=$(BUILD_DIR) \
		dbench-$(DBENCH_VERSION)/client.txt \
		dbench-$(DBENCH_VERSION)/dbench \
		dbench-$(DBENCH_VERSION)/README \
		dbench-$(DBENCH_VERSION)/tbench \
		dbench-$(DBENCH_VERSION)/tbench_srv
	$(TOUCH) $(STAGEFILES_DIR)/.dbench_packed

.PHONY: clean-dbench distclean-dbench

clean-dbench:
	-$(RM) -rf $(DBENCH_DIR)

#
# clean everthing else
#

distclean-dbench:
	-$(RM) -f $(STAGEFILES_DIR)/.dbench_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.dbench_patched
	-$(RM) -f $(STAGEFILES_DIR)/.dbench_configured
	-$(RM) -f $(STAGEFILES_DIR)/.dbench_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dbench_packed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(DBENCH_DLFILE)
endif
