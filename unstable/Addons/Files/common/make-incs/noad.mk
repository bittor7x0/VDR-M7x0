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
NOAD_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBMPEG2_INSTALLED)

ifeq ($(CONFIG_NOAD),y)
ifneq ($(CONFIG_LIBMPEG2),y)
   $(error dependency error: noad needs libmpeg2 enabled)
endif
endif

NOAD_VERSION := snapshot-20081021
NOAD_PATCHES_DIR := $(PATCHES_DIR)/noad/$(NOAD_VERSION)

NOAD_FILE := noad-$(NOAD_VERSION).tar.gz
NOAD_DLFILE := $(DOWNLOAD_DIR)/$(NOAD_FILE)
NOAD_URL := "http://git.gekrumbel.de/?p=noad.git;a=snapshot;h=e12eb6d8d02cc51d6e6dbe5e733009a354285700;sf=tgz"
NOAD_DIR := $(BUILD_DIR)/noad-$(NOAD_VERSION)

NOAD_INSTALLED = $(STAGEFILES_DIR)/.noad_installed

PACKS_RULES_$(CONFIG_NOAD) += $(NOAD_INSTALLED)
FILE_LISTS_$(CONFIG_NOAD) += noad.lst

CLEAN_RULES += clean-noad
DISTCLEAN_RULES += distclean-noad

#
# download noad
#

$(NOAD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NOAD_DLFILE) ] ; then \
	$(WGET) $(NOAD_URL) -O $(NOAD_DLFILE) ; \
	fi );
	$(TOUCH) $(NOAD_DLFILE)

#
# unpack noad
#

$(STAGEFILES_DIR)/.noad_unpacked: $(NOAD_DLFILE) \
                                           $(wildcard $(NOAD_PATCHES_DIR)/*.patch) \
                                           $$(NOAD_DEPS)
	-$(RM) -rf $(NOAD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(NOAD_DLFILE)
	$(MV) $(BUILD_DIR)/noad $(NOAD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.noad_unpacked

#
# patch noad
#

$(STAGEFILES_DIR)/.noad_patched: $(STAGEFILES_DIR)/.noad_unpacked
	$(call patch_package, $(NOAD_DIR), $(NOAD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.noad_patched

#
# configure noad
#

$(STAGEFILES_DIR)/.noad_configured: $(STAGEFILES_DIR)/.noad_patched
	($(CD) $(NOAD_DIR) ; $(UCLIBC_ENV) \
		$(NOAD_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--with-mpeginclude=$(TARGET_ROOT)/usr/include/mpeg2dec \
			--with-mpeglibdir=$(TARGET_ROOT)/usr/lib \
			--without-ffmpeg \
			--without-magick)
	$(TOUCH) $(STAGEFILES_DIR)/.noad_configured

#
# compile noad
#

$(STAGEFILES_DIR)/.noad_compiled: $(STAGEFILES_DIR)/.noad_configured
	$(UCLIBC_ENV) $(MAKE) -C $(NOAD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.noad_compiled

#
# install noad
#

$(STAGEFILES_DIR)/.noad_installed: $(STAGEFILES_DIR)/.noad_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(NOAD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.noad_installed


$(FILELIST_DIR)/noad.lst: $(STAGEFILES_DIR)/.noad_installed
	$(TOUCH) $(FILELIST_DIR)/noad.lst

.PHONY: clean-noad distclean-noad

clean-noad:
	-$(RM) -rf $(NOAD_DIR)

#
# clean everthing else
#

distclean-noad:
	-$(RM) -f $(STAGEFILES_DIR)/.noad_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.noad_patched
	-$(RM) -f $(STAGEFILES_DIR)/.noad_configured
	-$(RM) -f $(STAGEFILES_DIR)/.noad_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.noad_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NOAD_DLFILE)
endif
