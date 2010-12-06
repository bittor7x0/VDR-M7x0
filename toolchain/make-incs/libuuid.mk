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

ifeq ($(CONFIG_LIBUUID),y)
ifneq ($(CONFIG_E2FSPROGS),y)
   $(error dependency error: libuuid needs e2fsprogs enabled)
endif
endif

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBUUID_DEPS = $(BASE_BUILD_STAGEFILE) $(E2FSPROGS_INSTALLED)

LIBUUID_INSTALLED = $(STAGEFILES_DIR)/.libuuid_installed

PACKS_RULES_$(CONFIG_LIBUUID) += $(LIBUUID_INSTALLED)
FILE_LISTS_$(CONFIG_LIBUUID) += libuuid.lst

CLEAN_RULES += clean-e2fsprogs
DISTCLEAN_RULES += distclean-e2fsprogs

#
# install libuuid
#

$(STAGEFILES_DIR)/.libuuid_installed: $(TC_INIT_RULE)
	$(TOUCH) $(STAGEFILES_DIR)/.libuuid_installed


$(FILELIST_DIR)/libuuid.lst: $(STAGEFILES_DIR)/.libuuid_installed
	$(TOUCH) $(FILELIST_DIR)/libuuid.lst

.PHONY: clean-e2fsprogs distclean-e2fsprogs
