# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2012 VDR-NG-EM Project
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

OSD2TCP_DEPS = $(BASE_BUILD_STAGEFILE) $(ZLIB_INSTALLED)

ifeq ($(CONFIG_OSD2TCP),y)
ifneq ($(or $(CONFIG_ZLIB),$(CONFIG_ZLIB_STATIC)),y)
   $(error dependency error: osd2tcp needs zlib or zlib_static enabled)
endif
endif

OSD2TCP_DIR := $(BUILDIN_DIR)/osd2tcp
OSD2TCP_BUILDDIR := $(BUILD_DIR)/osd2tcp
OSD2TCP_INSTALLED = $(STAGEFILES_DIR)/.osd2tcp_installed

PACKS_RULES_$(CONFIG_OSD2TCP) += $(OSD2TCP_INSTALLED)
FILE_LISTS_$(CONFIG_OSD2TCP) += osd2tcp.lst

CLEAN_RULES += clean-osd2tcp
DISTCLEAN_RULES += distclean-osd2tcp


$(STAGEFILES_DIR)/.osd2tcp_copied: $(wildcard $(OSD2TCP_DIR)/*) \
   $$(OSD2TCP_DEPS)
	-$(RM) -rf $(OSD2TCP_BUILDDIR)
	$(CP) -RPp $(OSD2TCP_DIR) $(OSD2TCP_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.osd2tcp_copied

#
# compile osd2tcp
#

$(STAGEFILES_DIR)/.osd2tcp_compiled: $(STAGEFILES_DIR)/.osd2tcp_copied
	$(UCLIBC_ENV) $(MAKE) -C $(OSD2TCP_BUILDDIR) $(UCLIBC_ENV) \
		$(if $(CONFIG_ZLIB_STATIC), LDFLAGS="-static") \
		PREFIX=$(TARGET_ROOT) all
	$(TOUCH) $(STAGEFILES_DIR)/.osd2tcp_compiled

#
# install osd2tcp
#

$(STAGEFILES_DIR)/.osd2tcp_installed: $(STAGEFILES_DIR)/.osd2tcp_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(OSD2TCP_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) install
	$(TOUCH) $(STAGEFILES_DIR)/.osd2tcp_installed


$(FILELIST_DIR)/osd2tcp.lst: $(STAGEFILES_DIR)/.osd2tcp_installed
	$(TOUCH) $(FILELIST_DIR)/osd2tcp.lst

.PHONY: clean-osd2tcp distclean-osd2tcp

clean-osd2tcp:
	-$(RM) -rf $(OSD2TCP_BUILDDIR)

distclean-osd2tcp:
	-$(RM) -f $(STAGEFILES_DIR)/.osd2tcp_copied
	-$(RM) -f $(STAGEFILES_DIR)/.osd2tcp_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.osd2tcp_installed
