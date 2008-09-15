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
SCSI-SPIN_DEPS = $(BASE_BUILD_STAGEFILE)

SCSI-SPIN_VERSION := 0.9
SCSI-SPIN_PATCHES_DIR := $(PATCHES_DIR)/scsi-spin/$(SCSI-SPIN_VERSION)

#http://www.freewrt.org/distfiles/scsi-spin-0.9.tar.gz
SCSI-SPIN_FILE := scsi-spin-$(SCSI-SPIN_VERSION).tar.gz
SCSI-SPIN_DLFILE := $(DOWNLOAD_DIR)/$(SCSI-SPIN_FILE)
SCSI-SPIN_URL := http://www.freewrt.org/distfiles/$(SCSI-SPIN_FILE)
SCSI-SPIN_DIR := $(BUILD_DIR)/scsi-spin-$(SCSI-SPIN_VERSION)

SCSI-SPIN_INSTALLED = $(STAGEFILES_DIR)/.scsi-spin_installed

PACKS_RULES_$(CONFIG_SCSI-SPIN) += $(SCSI-SPIN_INSTALLED)
FILE_LISTS_$(CONFIG_SCSI-SPIN) += scsi-spin.lst

CLEAN_RULES += clean-scsi-spin
DISTCLEAN_RULES += distclean-scsi-spin

#
# download scsi-spin
#

$(SCSI-SPIN_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SCSI-SPIN_DLFILE) ] ; then \
	$(WGET) $(SCSI-SPIN_URL) -O $(SCSI-SPIN_DLFILE) ; \
	fi );
	$(TOUCH) $(SCSI-SPIN_DLFILE)

#
# unpack scsi-spin
#

$(STAGEFILES_DIR)/.scsi-spin_unpacked: $(SCSI-SPIN_DLFILE) \
                                           $(wildcard $(SCSI-SPIN_PATCHES_DIR)/*.patch) \
                                           $$(SCSI-SPIN_DEPS)
	-$(RM) -rf $(SCSI-SPIN_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SCSI-SPIN_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.scsi-spin_unpacked

#
# patch scsi-spin
#

$(STAGEFILES_DIR)/.scsi-spin_patched: $(STAGEFILES_DIR)/.scsi-spin_unpacked
	$(PATCHES_DIR)/patch-kernel.sh $(SCSI-SPIN_DIR) $(SCSI-SPIN_PATCHES_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.scsi-spin_patched

#
# compile scsi-spin
#

$(STAGEFILES_DIR)/.scsi-spin_compiled: $(STAGEFILES_DIR)/.scsi-spin_patched
	$(UCLIBC_ENV) $(MAKE) \
		-C $(SCSI-SPIN_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.scsi-spin_compiled

#
# install scsi-spin
#

$(STAGEFILES_DIR)/.scsi-spin_installed: $(STAGEFILES_DIR)/.scsi-spin_compiled
	$(CP) $(SCSI-SPIN_DIR)/scsi-spin $(TARGET_ROOT)/usr/sbin/scsi-spin
	chmod 755 $(TARGET_ROOT)/usr/sbin/scsi-spin
	$(TOUCH) $(STAGEFILES_DIR)/.scsi-spin_installed


$(FILELIST_DIR)/scsi-spin.lst: $(STAGEFILES_DIR)/.scsi-spin_installed
	$(TOUCH) $(FILELIST_DIR)/scsi-spin.lst

.PHONY: clean-scsi-spin distclean-scsi-spin

clean-scsi-spin:
	-$(RM) -rf $(SCSI-SPIN_DIR)

#
# clean everthing else
#

distclean-scsi-spin:
	-$(RM) -f $(STAGEFILES_DIR)/.scsi-spin_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.scsi-spin_patched
	-$(RM) -f $(STAGEFILES_DIR)/.scsi-spin_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.scsi-spin_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SCSI-SPIN_DLFILE)
endif
