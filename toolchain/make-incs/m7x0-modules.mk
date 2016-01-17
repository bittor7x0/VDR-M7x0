# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 VDR-NG-EM Project
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

M7X0-MODULES_DEPS = $(BASE_BUILD_STAGEFILE) $(CRAMFSCK_BIN)
ifeq ($(CONFIG_CRAMFS_NEEDS_SWAP),y)
  M7X0-MODULES_DEPS += $(CRAMFSSWAP_BIN)
endif

M7X0-MODULES_m740_SRC_LIST = \
    lib/modules/2.4.21-xfs/emma_rtos.o \
    lib/modules/2.4.21-xfs/emma_bios.o \
    lib/modules/2.4.21-xfs/emma_osd.o \
    lib/modules/2.4.21-xfs/emma_av.o \
    lib/modules/2.4.21-xfs/emma_teletext.o \
    lib/modules/2.4.21-xfs/emma_fb.o \
    lib/modules/2.4.21-xfs/iic_int.o \
    lib/modules/2.4.21-xfs/lirc_dev_s.o \
    lib/modules/2.4.21-xfs/dvbdev.o \
    lib/modules/2.4.21-xfs/pic16f73.o \
    lib/modules/2.4.21-xfs/pds5002cl.o \
    lib/modules/2.4.21-xfs/tda10046.o \
    lib/modules/2.4.21-xfs/tuner.o \
    lib/modules/2.4.21-xfs/demux.o \
    lib/modules/2.4.21-xfs/video.o

M7X0-MODULES_m750s_SRC_LIST = \
    lib/modules/2.4.21-xfs/emma_rtos.o \
    lib/modules/2.4.21-xfs/emma_bios.o \
    lib/modules/2.4.21-xfs/emma_osd.o \
    lib/modules/2.4.21-xfs/emma_av.o \
    lib/modules/2.4.21-xfs/emma_teletext.o \
    lib/modules/2.4.21-xfs/emma_fb.o \
    lib/modules/2.4.21-xfs/iic_int.o \
    lib/modules/2.4.21-xfs/lirc_dev_s.o \
    lib/modules/2.4.21-xfs/dvbdev.o \
    lib/modules/2.4.21-xfs/pic16f73.o \
    lib/modules/2.4.21-xfs/pds5002cl.o \
    lib/modules/2.4.21-xfs/tda10086.o \
    lib/modules/2.4.21-xfs/tuner.o \
    lib/modules/2.4.21-xfs/demux.o \
    lib/modules/2.4.21-xfs/video.o


M7X0-MODULES_DEST_DIR := lib/modules/2.4.21-xfs/tainted

M7X0-MODULES-FW-m740-VERSION := 2.4
M7X0-MODULES-FW-m750s-VERSION := 2.4
M7X0-MODULES_FILE := fw-siemens-$(CONFIG_M7X0_TYPE)-$(M7X0-MODULES-FW-$(CONFIG_M7X0_TYPE)-VERSION).zip
M7X0-MODULES_DLFILE := $(DOWNLOAD_DIR)/$(M7X0-MODULES_FILE)

ifeq ($(CONFIG_M7X0_TYPE),m740)
M7X0-MODULES_URL := https://www.assembla.com/spaces/VDR-EM-NG_Test/documents/bYjC8MpCSr5yujacwqEsg8/download/bYjC8MpCSr5yujacwqEsg8?notinline=true
else ifeq ($(CONFIG_M7X0_TYPE),m750s)
M7X0-MODULES_URL := http://gigaset.com/medias/sys_master/Gigaset_M750_S_V_2.4.exe
else
$(error unknown system type given)
endif

M7X0-MODULES_DIR := $(BUILD_DIR)/m7x0-modules
M7X0-MODULES-FW-m740-WSW := $(M7X0-MODULES_DIR)/2.4_t.wsw
M7X0-MODULES-FW-m750s-WSW = $(M7X0-MODULES_DIR)/2.4_sat.wsw
M7X0-MODULES-FW-CRAM := $(M7X0-MODULES_DIR)/wsw-cram-aligned-$(CONFIG_M7X0_TYPE)
M7X0-MODULES-FW-ROOTEXT-DIR := $(M7X0-MODULES_DIR)/root_$(CONFIG_M7X0_TYPE)_extracted


M7X0-MODULES_INSTALLED = $(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_installed

PACKS_RULES_$(CONFIG_M7X0-MODULES) += $(M7X0-MODULES_INSTALLED)
FILE_LISTS_$(CONFIG_M7X0-MODULES) += m7x0-modules-$(CONFIG_M7X0_TYPE).lst

CLEAN_RULES += clean-m7x0-modules
DISTCLEAN_RULES += distclean-m7x0-modules

#
# download fw
#

$(M7X0-MODULES_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(M7X0-MODULES_DLFILE) ] ; then \
	$(WGET) $(M7X0-MODULES_URL) -O $(M7X0-MODULES_DLFILE) ; \
	fi );
	$(TOUCH) $(M7X0-MODULES_DLFILE)

#
# unpack fw
#

$(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_unpacked: $(M7X0-MODULES_DLFILE) \
                                                              $$(M7X0-MODULES_DEPS)
	-$(RM) -rf $(M7X0-MODULES_DIR)
	$(MKDIR) -p $(M7X0-MODULES_DIR)
	(cd $(M7X0-MODULES_DIR) ; $(UNZIP) -x $(M7X0-MODULES_DLFILE))
	$(DD) bs=1 if=$(M7X0-MODULES-FW-$(CONFIG_M7X0_TYPE)-WSW) \
		of=$(M7X0-MODULES-FW-CRAM) skip=1416 count=$(CRAMFS_MAX_IMGSIZE)
ifeq ($(CONFIG_CRAMFS_NEEDS_SWAP),y)
	-$(CRAMFSSWAP_BIN) $(M7X0-MODULES-FW-CRAM) $(M7X0-MODULES-FW-CRAM).host
ifeq ($(CONFIG_M7X0_TYPE),m750s)
	$(SECHO) -----------------------------------------------
	$(SECHO) THX DISCVISION FOR TRUNCATING THE M750S CRAMFS.
	$(SECHO) THIS BREAKS ALMOST EVERYTHING TO GET THE BLOBBY 
	$(SECHO) SHIT OUT OF YOUR CREEPY FIRMWARE!!!!!
	$(SECHO) ----------------------------------------------- 
	$(DD) if=/dev/zero bs=1 count=5780 >> $(M7X0-MODULES-FW-CRAM).host
endif
else 
	$(CP) $(M7X0-MODULES-FW-CRAM) $(M7X0-MODULES-FW-CRAM).host
endif
	-$(RM) -rf $(M7X0-MODULES-FW-ROOTEXT-DIR)
	-$(CRAMFSCK_BIN) -r -x $(M7X0-MODULES-FW-ROOTEXT-DIR) \
		$(M7X0-MODULES-FW-CRAM).host
	$(TOUCH) $(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_unpacked

#
# install modules
#

m7x0-modules_cp =  $(PREFIX_BIN)/$(UCLIBC_OBJCOPY) -O elf32-tradbigmips \
   $(M7X0-MODULES-FW-ROOTEXT-DIR)/$(strip $(1)) \
   $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/$(M7X0-MODULES_DEST_DIR)/$(notdir $(1));

$(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_installed: \
     $(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_unpacked
	-$(RM) -rf $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/$(M7X0-MODULES_DEST_DIR)
	$(MKDIR) -p $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/$(M7X0-MODULES_DEST_DIR)
	$(foreach mod,$(M7X0-MODULES_$(CONFIG_M7X0_TYPE)_SRC_LIST), \
		$(call m7x0-modules_cp,$(mod)))
	$(TOUCH) $(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_installed

#
# generate rootfs file list
#

m7x0-modules_lst_dir = \
  $(ECHO) "$(1)       -                 d 755 0 0 - - - - -"; \
  $(eval m7x0-modules_lst_dir_accu := $(1)/)

m7x0-modules_lst_mods = \
  $(ECHO) "$(M7X0-MODULES_DEST_DIR)/$(notdir $(1))" \
  "$(M7X0_KERNEL_DIR)/$(M7X0-MODULES_DEST_DIR)/$(notdir $(1)) f 755 0 0 - - - - -";

$(FILELIST_DIR)/m7x0-modules-$(CONFIG_M7X0_TYPE).lst: \
      $(STAGEFILES_DIR)/.m7x0-modules-$(CONFIG_M7X0_TYPE)_installed
	$(eval m7x0-modules_lst_dir_accu := )
	($(foreach dir,$(subst /, ,$(M7X0-MODULES_DEST_DIR)), \
		$(call m7x0-modules_lst_dir,$(m7x0-modules_lst_dir_accu)$(dir))) \
	$(foreach mod, $(M7X0-MODULES_$(CONFIG_M7X0_TYPE)_SRC_LIST), \
		$(call m7x0-modules_lst_mods, $(mod)))) \
	> $(FILELIST_DIR)/m7x0-modules-$(CONFIG_M7X0_TYPE).lst

.PHONY: clean-m7x0-modules distclean-m7x0-modules


clean-m7x0-modules:
	-$(RM) -rf $(M7X0-MODULES_DIR)

distclean-m7x0-modules:
	-$(RM) -f $(STAGEFILES_DIR)/.m7x0-modules-*
	-$(RM) -f $(FILELIST_DIR)/m7x0-modules-*.lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(M7X0-MODULES_DLFILE)
endif
