# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2018 VDR-NG-EM Project
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

SYS-CONFIGS_BASE_DIR := $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)
SYS-CONFIGS_COMMON_DIR := $(SYS-CONFIGS_BASE_DIR)/common
SYS-CONFIGS_SYSTYPE_DIR := $(SYS-CONFIGS_BASE_DIR)/$(CONFIG_M7X0_TYPE)
SYS-CONFIGS_COMMON_LST := sys-configs-$(subst /,-,$(CONFIG_SCRIPT_BASE))-common.lst
SYS-CONFIGS_SYSTYPE_LST := sys-configs-$(subst /,-,$(CONFIG_SCRIPT_BASE))-$(CONFIG_M7X0_TYPE).lst

FILE_LISTS_y += $(SYS-CONFIGS_COMMON_LST)
FILE_LISTS_y += $(SYS-CONFIGS_SYSTYPE_LST)
DISTCLEAN_RULES += distclean-sys-configs

$(eval $(call gen_copy_file_lst,$(SYS-CONFIGS_COMMON_DIR),,0,0,$(SYS-CONFIGS_COMMON_LST),check))

$(eval $(call gen_copy_file_lst,$(SYS-CONFIGS_SYSTYPE_DIR),,0,0,$(SYS-CONFIGS_SYSTYPE_LST),check))

.PHONY: distclean-sys-configs

distclean-sys-configs:
	-$(RM) -f $(FILELIST_DIR)/$(SYS-CONFIGS_COMMON_LST)
	-$(RM) -f $(FILELIST_DIR)/$(SYS-CONFIGS_SYSTYPE_LST)
