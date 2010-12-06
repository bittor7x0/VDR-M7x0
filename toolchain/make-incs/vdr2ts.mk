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
# The project's page is at http://www.assembla.com/wiki/show/VDR-M7x0
#

VDR2TS_DEPS = $(BASE_BUILD_STAGEFILE)

VDR2TS_DIR := $(BUILDIN_DIR)/vdr2ts
VDR2TS_BUILDDIR := $(BUILD_DIR)/vdr2ts
VDR2TS_INSTALLED = $(STAGEFILES_DIR)/.vdr2ts_installed

PACKS_RULES_$(CONFIG_VDR2TS) += $(VDR2TS_INSTALLED)
FILE_LISTS_$(CONFIG_VDR2TS) += vdr2ts.lst

CLEAN_RULES += clean-vdr2ts
DISTCLEAN_RULES += distclean-vdr2ts


$(STAGEFILES_DIR)/.vdr2ts_copied: $(wildcard $(VDR2TS_DIR)/*) \
   $$(VDR2TS_DEPS)
	-$(RM) -rf $(VDR2TS_BUILDDIR)
	$(CP) -RPp $(VDR2TS_DIR) $(VDR2TS_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.vdr2ts_copied

#
# compile vdr2ts
#

$(STAGEFILES_DIR)/.vdr2ts_compiled: $(STAGEFILES_DIR)/.vdr2ts_copied
	$(UCLIBC_ENV) $(MAKE) -C $(VDR2TS_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) all
	$(TOUCH) $(STAGEFILES_DIR)/.vdr2ts_compiled

#
# install vdr2ts
#

$(STAGEFILES_DIR)/.vdr2ts_installed: $(STAGEFILES_DIR)/.vdr2ts_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(VDR2TS_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) install
	$(TOUCH) $(STAGEFILES_DIR)/.vdr2ts_installed


$(FILELIST_DIR)/vdr2ts.lst: $(STAGEFILES_DIR)/.vdr2ts_installed
	$(TOUCH) $(FILELIST_DIR)/vdr2ts.lst

.PHONY: clean-vdr2ts distclean-vdr2ts

clean-vdr2ts:
	-$(RM) -rf $(VDR2TS_BUILDDIR)

distclean-vdr2ts:
	-$(RM) -f $(STAGEFILES_DIR)/.vdr2ts_copied
	-$(RM) -f $(STAGEFILES_DIR)/.vdr2ts_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.vdr2ts_installed

