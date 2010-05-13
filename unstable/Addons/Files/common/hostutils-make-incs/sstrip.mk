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


SSTRIP_HOSTDIR := $(BUILDIN_DIR)/sstrip
SSTRIP_HOSTBUILDDIR := $(HOSTUTILS_BUILD_DIR)/sstrip
SSTRIP_HOSTINSTALLED = $(STAGEFILES_DIR)/.sstrip_host_installed

CLEAN_RULES += clean-sstrip
DISTCLEAN_RULES += distclean-sstrip

$(STAGEFILES_DIR)/.sstrip_host_copied: $(wildcard $(SSTRIP_HOSTDIR)/*) $(TC_INIT_RULE)
	-$(RM) -rf $(SSTRIP_HOSTBUILDDIR)
	$(MKDIR) -p $(SSTRIP_HOSTBUILDDIR)
	$(CP) -RPp $(SSTRIP_HOSTDIR)/* $(SSTRIP_HOSTBUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.sstrip_host_copied

#
# compile sstrip
#

$(STAGEFILES_DIR)/.sstrip_host_compiled: $(STAGEFILES_DIR)/.sstrip_host_copied
	$(MAKE) -C $(SSTRIP_HOSTBUILDDIR) distclean
	$(MAKE) -C $(SSTRIP_HOSTBUILDDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.sstrip_host_compiled

#
# install sstrip
#

$(STAGEFILES_DIR)/.sstrip_host_installed: $(STAGEFILES_DIR)/.sstrip_host_compiled
	$(MAKE) -C $(SSTRIP_HOSTBUILDDIR) SBINDIR=$(PREFIX_BIN) install
	$(TOUCH) $(STAGEFILES_DIR)/.sstrip_host_installed


.PHONY: clean-sstrip distclean-sstrip

clean-sstrip:
	-$(RM) -rf $(SSTRIP_HOSTBUILDDIR)

distclean-sstrip:
	-$(RM) -f $(STAGEFILES_DIR)/.sstrip_host_copied
	-$(RM) -f $(STAGEFILES_DIR)/.sstrip_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.sstrip_host_installed

