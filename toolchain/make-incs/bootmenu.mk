# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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

BOOTMENU_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBOSD_INSTALLED)

ifeq ($(CONFIG_BOOTMENU),y)
ifneq ($(CONFIG_LIBOSD),y)
   $(error dependency error: bootmenu needs libosd enabled)
endif
endif

PTYPES_VERSION := ptypes-2.1.1
PTYPES_FILE := $(PTYPES_VERSION).tar.gz
PTYPES_URL := http://www.melikyan.com/ptypes/ptypes-2.1.1.tar.gz
PTYPES_DLFILE := $(DOWNLOAD_DIR)/$(PTYPES_FILE)

BOOTMENU_BRANCH := tags/bootmenu/REL_0.2/src
#BOOTMENU_BRANCH := trunk/bootmenu/src
BOOTMENU_SVN := svn://open7x0.org/boxamp
BOOTMENU_DIR := $(BUILD_DIR)/bootmenu
BOOTMENU_TC_FILE := $(BOOTMENU_DIR)/linux-mips-uclibc.tc

LASTDIR := `pwd`

PACKS_RULES_$(CONFIG_BOOTMENU) += $(BOOTMENU_INSTALLED)
FILE_LISTS_$(CONFIG_BOOTMENU) += bootmenu.lst

CLEAN_RULES += clean-bootmenu
DISTCLEAN_RULES += distclean-bootmenu

#
# download ptypes
#
$(PTYPES_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PTYPES_DLFILE) ] ; then \
	$(WGET) $(PTYPES_URL) -O $(PTYPES_DLFILE) ; \
	fi );
	$(TOUCH) $(PTYPES_DLFILE)

#
# unpack ptypes
#
$(STAGEFILES_DIR)/.bootmenu_unpacked: $(PTYPES_DLFILE)
	$(RM) -rf $(BOOTMENU_DIR)
	$(MKDIR) $(BOOTMENU_DIR)
	$(TAR) -C $(BOOTMENU_DIR) -zf $(PTYPES_DLFILE)
	$(MKDIR) $(BOOTMENU_DIR)/src
	$(CD) $(BOOTMENU_DIR)/$(PTYPES_VERSION)/src && (for f in *.cxx; do cp "$$f" $(BOOTMENU_DIR)/src/"$${f%.cxx}.cpp"; done);
	$(CP) $(BOOTMENU_DIR)/$(PTYPES_VERSION)/include/*.h $(BOOTMENU_DIR)/src
	$(TOUCH) $(STAGEFILES_DIR)/.bootmenu_unpacked

#
# checkout bootmenu
#
$(STAGEFILES_DIR)/.bootmenu_checkedout: $(STAGEFILES_DIR)/.bootmenu_unpacked
	$(CD) $(BOOTMENU_DIR) && $(SVN) co ${BOOTMENU_SVN}/${BOOTMENU_BRANCH}
	$(TOUCH) $(STAGEFILES_DIR)/.bootmenu_checkedout

#
# compile ptypes and bootmenu
#
$(STAGEFILES_DIR)/.bootmenu_compiled: $(STAGEFILES_DIR)/.bootmenu_checkedout $$(BOOTMENU_DEPS)
	$(CD) $(BOOTMENU_DIR)/src && env $(UCLIBC_ENV) $(MAKE) -f Makefile.tc $(UCLIBC_ENV)
	$(TOUCH) $(STAGEFILES_DIR)/.bootmenu_compiled

#
# install bootmenu
#
$(STAGEFILES_DIR)/.bootmenu_installed: $(STAGEFILES_DIR)/.bootmenu_compiled
	$(CP) $(BOOTMENU_DIR)/src/bootmenu $(TARGET_ROOT)/usr/bin/bootmenu
	$(TOUCH) $(STAGEFILES_DIR)/.bootmenu_installed

$(FILELIST_DIR)/bootmenu.lst: $(STAGEFILES_DIR)/.bootmenu_installed
	$(TOUCH) $(FILELIST_DIR)/bootmenu.lst

.PHONY: clean-bootmenu distclean-bootmenu

clean-bootmenu:
	(if [ -f $(BOOTMENU_DIR)/src ] ; then \
		$(CD) $(BOOTMENU_DIR)/src && $(MAKE) -f Makefile.tc clean ; \
	fi );
	
distclean-bootmenu:
	-$(RM) -rf $(BOOTMENU_DIR)
	-$(RM) -f $(STAGEFILES_DIR)/.bootmenu_checkedout
	-$(RM) -f $(STAGEFILES_DIR)/.bootmenu_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.bootmenu_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.bootmenu_installed
