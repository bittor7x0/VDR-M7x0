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

READLINE_DEPS = $(BASE_BUILD_STAGEFILE) $(NCURSES_INSTALLED)

ifeq ($(CONFIG_READLINE),y)
ifneq ($(CONFIG_NCURSES),y)
   $(error dependency error: readline needs ncurses enabled)
endif
endif

READLINE_VERSION := 6.1
READLINE_PATCHES_DIR := $(PATCHES_DIR)/readline/$(READLINE_VERSION)

READLINE_FILE := readline-$(READLINE_VERSION).tar.gz
READLINE_DLFILE := $(DOWNLOAD_DIR)/$(READLINE_FILE)
READLINE_DIR := $(BUILD_DIR)/readline-$(READLINE_VERSION)
READLINE_BUILD_DIR := $(BUILD_DIR)/readline.build
READLINE_URL := ftp://ftp.gnu.org/gnu/readline/$(READLINE_FILE)

READLINE_INSTALLED = $(STAGEFILES_DIR)/.readline_installed

PACKS_RULES_$(CONFIG_READLINE) += $(READLINE_INSTALLED)
FILE_LISTS_$(CONFIG_READLINE) += readline.lst

CLEAN_RULES += clean-readline
DISTCLEAN_RULES += distclean-readline

#
# download readline
#

$(READLINE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(READLINE_DLFILE) ] ; then \
	$(WGET) $(READLINE_URL) -O $(READLINE_DLFILE) ; \
	fi );
	$(TOUCH) $(READLINE_DLFILE)

#
# unpack readline
#

$(STAGEFILES_DIR)/.readline_unpacked: $(READLINE_DLFILE) \
                                      $(wildcard $(READLINE_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(READLINE_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(READLINE_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.readline_unpacked

#
# patch readline
#

$(STAGEFILES_DIR)/.readline_patched: $(STAGEFILES_DIR)/.readline_unpacked
	$(call patch_package, $(READLINE_DIR), $(READLINE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.readline_patched

#
# configure readline
#

$(STAGEFILES_DIR)/.readline_configured: $(STAGEFILES_DIR)/.readline_patched \
                                        $$(READLINE_DEPS)
	-$(RM) -rf $(READLINE_BUILD_DIR)
	$(MKDIR) -p $(READLINE_BUILD_DIR)
	($(CD) $(READLINE_BUILD_DIR) ; $(UCLIBC_ENV) \
		$(READLINE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--libdir=$(TARGET_ROOT)/lib \
			--disable-static \
			--host=$(TARGET) \
			--with-curses)
	$(TOUCH) $(STAGEFILES_DIR)/.readline_configured

#
# compile readline
#

$(STAGEFILES_DIR)/.readline_compiled: $(STAGEFILES_DIR)/.readline_configured
	$(UCLIBC_ENV) $(MAKE) -C $(READLINE_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.readline_compiled

#
# install readline
#

$(STAGEFILES_DIR)/.readline_installed: $(STAGEFILES_DIR)/.readline_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(READLINE_BUILD_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.readline_installed

$(FILELIST_DIR)/readline.lst: $(STAGEFILES_DIR)/.readline_installed
	$(TOUCH) $(FILELIST_DIR)/readline.lst

.PHONY: clean-readline distclean-readline

clean-readline:
	-$(RM) -rf $(READLINE_BUILD_DIR)
	-$(RM) -rf $(READLINE_DIR)

distclean-readline:
	-$(RM) -f $(STAGEFILES_DIR)/.readline_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.readline_patched
	-$(RM) -f $(STAGEFILES_DIR)/.readline_configured
	-$(RM) -f $(STAGEFILES_DIR)/.readline_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.readline_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(READLINE_DLFILE)
endif
