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

TOOL-BOX-EXTRA_DEPS = $(BASE_BUILD_STAGEFILE)

TOOL-BOX-EXTRA_DIR := $(BUILDIN_DIR)/tool-box-extra
TOOL-BOX-EXTRA_BUILDDIR := $(BUILD_DIR)/tool-box-extra
TOOL-BOX-EXTRA_INSTALLED = $(STAGEFILES_DIR)/.tool-box-extra_installed

PACKS_RULES_$(CONFIG_TOOL-BOX-EXTRA) += $(TOOL-BOX-EXTRA_INSTALLED)
FILE_LISTS_$(CONFIG_TOOL-BOX-EXTRA) += tool-box-extra.lst

CLEAN_RULES += clean-tool-box-extra
DISTCLEAN_RULES += distclean-tool-box-extra


$(STAGEFILES_DIR)/.tool-box-extra_copied: $(wildcard $(TOOL-BOX-EXTRA_DIR)/*) \
   $$(TOOL-BOX-EXTRA_DEPS)
	-$(RM) -rf $(TOOL-BOX-EXTRA_BUILDDIR)
	$(CP) -RPp $(TOOL-BOX-EXTRA_DIR) $(TOOL-BOX-EXTRA_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box-extra_copied

#
# compile tool-box-extra
#

$(STAGEFILES_DIR)/.tool-box-extra_compiled: $(STAGEFILES_DIR)/.tool-box-extra_copied
	$(UCLIBC_ENV) $(MAKE) -C $(TOOL-BOX-EXTRA_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT)/usr all
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box-extra_compiled

#
# install tool-box-extra
#

$(STAGEFILES_DIR)/.tool-box-extra_installed: $(STAGEFILES_DIR)/.tool-box-extra_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(TOOL-BOX-EXTRA_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT)/usr install
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box-extra_installed


$(FILELIST_DIR)/tool-box-extra.lst: $(STAGEFILES_DIR)/.tool-box-extra_installed
	$(TOUCH) $(FILELIST_DIR)/tool-box-extra.lst

.PHONY: clean-tool-box-extra distclean-tool-box-extra

clean-tool-box-extra:
	-$(RM) -rf $(TOOL-BOX-EXTRA_BUILDDIR)

distclean-tool-box-extra:
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box-extra_copied
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box-extra_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box-extra_installed

