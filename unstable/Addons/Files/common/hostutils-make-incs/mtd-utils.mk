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


MKJFFS2_BIN := $(HOSTUTILS_PREFIX_BIN)/mkfs.jffs2


MTD-UTILS_HOSTVERSION := 20050122
MTD-UTILS_HOSTFILE := mtd_$(MTD-UTILS_HOSTVERSION).orig.tar.gz
MTD-UTILS_HOSTDLFILE := $(DOWNLOAD_DIR)/$(MTD-UTILS_HOSTFILE)

MTD-UTILS_HOSTPATCHES_DIR := $(PATCHES_DIR)/mtd-utils/$(MTD-UTILS_HOSTVERSION)
MTD-UTILS_HOSTURL := http://downloads.openwrt.org/sources/$(MTD-UTILS_HOSTFILE)
MTD-UTILS_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/mtd-$(MTD-UTILS_HOSTVERSION).orig

CFLAGS := $(CFLAGS) -I$(BUILDIN_DIR)/mtd-utils -I$(MTD-UTILS_HOSTDIR)/include
ifneq ($(HOST_BS),Linux)
CFLAGS += -Dloff_t=off_t -D__BYTE_ORDER=BYTE_ORDER -include getline.h -include endian.h
endif

CLEAN_RULES += clean-mtd-utils-host
DISTCLEAN_RULES += distclean-mtd-utils-host

#
# download mtd-utils
#

$(MTD-UTILS_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MTD-UTILS_HOSTDLFILE) ] ; then \
	$(WGET) $(MTD-UTILS_HOSTURL) -O $(MTD-UTILS_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(MTD-UTILS_HOSTDLFILE)

#
# unpack mtd-utils
#

$(STAGEFILES_DIR)/.mtd-utils_host_unpacked: $(MTD-UTILS_HOSTDLFILE) \
                                         $(wildcard $(MTD-UTILS_HOSTPATCHES_DIR)/*.patch)
	-$(RM) -rf $(MTD-UTILS_HOSTDIR)
	$(TAR) -C $(HOSTUTILS_BUILD_DIR) -zf $(MTD-UTILS_HOSTDLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mtd-utils_host_unpacked

#
# patch mtd-utils
#

$(STAGEFILES_DIR)/.mtd-utils_host_patched: $(STAGEFILES_DIR)/.mtd-utils_host_unpacked
	$(call patch_package, $(MTD-UTILS_HOSTDIR), $(MTD-UTILS_HOSTPATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mtd-utils_host_patched

#
# compile mtd-utils
#

$(STAGEFILES_DIR)/.mtd-utils_host_compiled: $(STAGEFILES_DIR)/.mtd-utils_host_patched
	$(MAKE) -C $(MTD-UTILS_HOSTDIR)/util \
		LINUXDIR="$(LINUX_HEADERS_DIR)" \
		CFLAGS="$(CFLAGS)" \
		TARGETS=mkfs.jffs2
	$(TOUCH) $(STAGEFILES_DIR)/.mtd-utils_host_compiled

#
# install mtd-utils
#

$(HOSTUTILS_PREFIX_BIN)/mkfs.jffs2: $(STAGEFILES_DIR)/.mtd-utils_host_compiled
	$(CP) $(MTD-UTILS_HOSTDIR)/util/mkfs.jffs2 $(HOSTUTILS_PREFIX_BIN)/mkfs.jffs2

.PHONY: clean-mtd-utils-host distclean-mtd-utils-host

clean-mtd-utils-host:
	-$(RM) -rf $(MTD-UTILS_HOSTDIR)

distclean-mtd-utils-host:
	-$(RM) -f $(STAGEFILES_DIR)/.mtd-utils_host_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mtd-utils_host_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mtd-utils_host_compiled
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/mkfs.jffs2
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MTD-UTILS_HOSTDLFILE)
endif
