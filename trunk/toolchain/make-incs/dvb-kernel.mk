# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2014 VDR-NG-EM Project
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

DVB-KERNEL_DEPS = $(BASE_BUILD_STAGEFILE) $(SIEMENS-LINUX-KERNEL_INSTALLED)

ifeq ($(CONFIG_DVB-KERNEL),y)
ifneq ($(CONFIG_SIEMENS-LINUX-KERNEL),y)
   $(error dependency error: dvb-kernel needs siemens-linux-kernel enabled)
endif
endif

DVB-KERNEL_SVN_URL := http://svn.assembla.com/svn/VDR-M7x0/trunk/dvb-kernel-m7x0
DVB-KERNEL_DIR := $(BUILD_DIR)/dvb-kernel-m7x0
DVB-KERNEL_CONF_DIR := $(PRG_CONFIGS_DIR)/dvb-kernel
DVB-KERNEL_INSTALLED = $(STAGEFILES_DIR)/.dvb-kernel_installed

PACKS_RULES_$(CONFIG_DVB-KERNEL) += $(DVB-KERNEL_INSTALLED)
FILE_LISTS_$(CONFIG_DVB-KERNEL) += dvb-kernel.lst dvb-kernel-modules.lst dvb-kernel-configs.lst

CLEAN_RULES += clean-dvb-kernel
DISTCLEAN_RULES += distclean-dvb-kernel

DVB-KERNEL_MODLST := \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/v4l1-compat.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/v4l2-common.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/video-buf.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/dvb-core.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/ves1x93.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/alps_tdlb7.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/alps_tdmb7.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/stv0299.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/ves1820.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/tda1004x.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/grundig_29504-401.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/grundig_29504-491.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/cx24110.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/mt312.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/saa7146.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/saa7146_vv.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/ttpci-eeprom.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/dvb-ttusb-budget.o \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb/ttusb_dec.o

DVB-KERNEL_DIRLST := \
   lib/modules \
   lib/modules/2.4.21-xfs \
   lib/modules/2.4.21-xfs/kernel \
   lib/modules/2.4.21-xfs/kernel/drivers \
   lib/modules/2.4.21-xfs/kernel/drivers/dvb

ifneq ($(and $(filter y,$(CONFIG_DVB-KERNEL)), $(wildcard $(DVB-KERNEL_DIR))),)
$(info Updating dvb-kernel download ...)
dvb-kernel_svn_changed := $(call svn_changed, $(DVB-KERNEL_DIR))

ifeq ($(dvb-kernel_svn_changed),0)
$(info dvb-kernel is up to date)
else
$(info $(dvb-kernel_svn_changed) file(s) updated)
dvb-kernel_tmp := $(shell $(TOUCH) $(STAGEFILES_DIR)/.dvb-kernel_downloaded)
endif
endif

#
# download dvb-kernel
#

$(STAGEFILES_DIR)/.dvb-kernel_downloaded: $(TC_INIT_RULE)
	$(SVN) co $(DVB-KERNEL_SVN_URL) $(DVB-KERNEL_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.dvb-kernel_downloaded

#
# compile dvb-kernel
#

$(STAGEFILES_DIR)/.dvb-kernel_compiled: $(STAGEFILES_DIR)/.dvb-kernel_downloaded $$(DVB-KERNEL_DEPS)
	($(CD) $(DVB-KERNEL_DIR)/build-2.4 ; \
		$(CP) -f "$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/lib/modules/2.4.21-xfs/build/include/linux/videodev.h" videodev.h ; \
		$(CP) -f "$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/lib/modules/2.4.21-xfs/build/.config" .config ; \
		$(LN) -sf linux/include ../include ; \
		./getlinks ; \
		$(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		PATH="$(PREFIX_BIN):$(PATH)" DESTDIR="$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)" \
		HOSTCC=$(CC) CC=$(EGCS_BIN) -C $(DVB-KERNEL_DIR)/build-2.4)
	$(TOUCH) $(STAGEFILES_DIR)/.dvb-kernel_compiled

#
# install dvb-kernel
#

$(STAGEFILES_DIR)/.dvb-kernel_installed: $(STAGEFILES_DIR)/.dvb-kernel_compiled
	($(CD) $(DVB-KERNEL_DIR)/build-2.4 ; $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		PATH="$(PREFIX_BIN):$(PATH)" DESTDIR="$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)" \
		HOSTCC=$(CC) CC=$(EGCS_BIN) -C $(DVB-KERNEL_DIR)/build-2.4 install)
	$(TOUCH) $(STAGEFILES_DIR)/.dvb-kernel_installed


$(FILELIST_DIR)/dvb-kernel.lst: $(STAGEFILES_DIR)/.dvb-kernel_installed
	$(TOUCH) $(FILELIST_DIR)/dvb-kernel.lst

#
# generate rootfs file list
#

$(FILELIST_DIR)/dvb-kernel-modules.lst: $(STAGEFILES_DIR)/.dvb-kernel_installed
	($(foreach dir,$(DVB-KERNEL_DIRLST),\
		$(call siemens-linux-kernel_lst_dir,$(dir))) \
	 $(foreach mod,$(DVB-KERNEL_MODLST),\
		$(call siemens-linux-kernel_lst_mod,$(mod)))) \
			> $(FILELIST_DIR)/dvb-kernel-modules.lst

$(eval $(call gen_copy_file_lst,$(DVB-KERNEL_CONF_DIR),,0,0,dvb-kernel-configs.lst,check))

.PHONY: clean-dvb-kernel distclean-dvb-kernel

clean-dvb-kernel:
	-$(RM) -rf $(DVB-KERNEL_DIR)

distclean-dvb-kernel:
	-$(RM) -f $(STAGEFILES_DIR)/.dvb-kernel_downloaded
	-$(RM) -f $(STAGEFILES_DIR)/.dvb-kernel_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.dvb-kernel_installed
	-$(RM) -f $(FILELIST_DIR)/dvb-kernel-modules.lst
	-$(RM) -f $(FILELIST_DIR)/dvb-kernel-configs.lst