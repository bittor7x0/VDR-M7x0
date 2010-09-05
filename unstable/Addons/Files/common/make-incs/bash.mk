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

BASH_DEPS = $(BASE_BUILD_STAGEFILE) $(NCURSES_INSTALLED) $(READLINE_INSTALLED)

ifeq ($(CONFIG_BASH),y)
ifneq ($(and $(filter y,$(CONFIG_NCURSES)),$(filter y,$(CONFIG_READLINE))),y)
   $(error dependency error: bash needs readline and ncurses enabled)
endif
endif

BASH_VERSION := 4.1
BASH_PATCHES_DIR := $(PATCHES_DIR)/bash/$(BASH_VERSION)

BASH_FILE := bash-$(BASH_VERSION).tar.gz
BASH_DLFILE := $(DOWNLOAD_DIR)/$(BASH_FILE)
BASH_DIR := $(BUILD_DIR)/bash-$(BASH_VERSION)
BASH_BUILD_DIR := $(BUILD_DIR)/bash.build
BASH_URL := ftp://ftp.gnu.org/gnu/bash/$(BASH_FILE)

BASH_INSTALLED = $(STAGEFILES_DIR)/.bash_installed

PACKS_RULES_$(CONFIG_BASH) += $(READLINE_INSTALLED)
FILE_LISTS_$(CONFIG_BASH) += bash.lst

CLEAN_RULES += clean-bash
DISTCLEAN_RULES += distclean-bash

#
# download bash
#

$(BASH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(BASH_DLFILE) ] ; then \
	$(WGET) $(BASH_URL) -O $(BASH_DLFILE) ; \
	fi );
	$(TOUCH) $(BASH_DLFILE)


#
# unpack bash
#

$(STAGEFILES_DIR)/.bash_unpacked: $(BASH_DLFILE) $(BASH_PATCHES_DIR)/*.patch
	-rm -rf $(BASH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(BASH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.bash_unpacked

#
# patch bash
#

$(STAGEFILES_DIR)/.bash_patched: $(STAGEFILES_DIR)/.bash_unpacked
	$(call patch_package, $(BASH_DIR), $(BASH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.bash_patched

#
# configure bash
#

$(STAGEFILES_DIR)/.bash_configured: $(STAGEFILES_DIR)/.bash_patched \
                                    $$(BASH_DEPS)
	-$(RM) -rf $(BASH_BUILD_DIR)
	$(MKDIR) -p $(BASH_BUILD_DIR)
	($(CD) $(BASH_BUILD_DIR) ; $(UCLIBC_ENV) \
		ac_cv_func_setvbuf_reversed=no \
		ac_cv_rl_version=5.2 \
		bash_cv_getcwd_malloc=yes \
		bash_cv_func_ctype_nonascii=yes \
		bash_cv_job_control_missing=present \
		bash_cv_sys_named_pipes=present \
		bash_cv_sys_errlist=yes \
		bash_cv_sys_siglist=yes \
		bash_cv_func_sigsetjmp=present \
		bash_cv_must_reinstall_sighandlers=yes \
		bash_cv_unusable_rtsigs=no \
		$(BASH_DIR)/configure \
			--prefix=$(TARGET_ROOT)/ \
			--enable-alias \
			--host=$(TARGET) \
			--enable-arith-for-command \
			--enable-array-variables \
			--enable-brace-expansion \
			--enable-cond-command \
			--enable-cond-regexp \
			--enable-directory-stack \
			--disable-disabled-builtins \
			--enable-dparen-arithmetic \
			--disable-help-builtin \
			--enable-history \
			--enable-job-control \
			--disable-multibyte \
			--enable-net-redirections \
			--disable-progcomp \
			--enable-prompt-string-decoding \
			--disable-restricted \
			--enable-select \
			--disable-separate-helpfiles \
			--disable-single-help-strings \
			--disable-strict-posix-default \
			--disable-mem-scramble \
			--disable-profiling \
			--disable-static-link \
			--enable-largefile \
			--disable-nls \
			--disable-rpath \
			--without-bash-malloc \
			--with-curses \
			--with-installed-readline\
			--with-gnu-ld)
	$(TOUCH) $(STAGEFILES_DIR)/.bash_configured

#
# compile bash
#

$(STAGEFILES_DIR)/.bash_compiled: $(STAGEFILES_DIR)/.bash_configured
	$(UCLIBC_ENV) $(MAKE) -C $(BASH_BUILD_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.bash_compiled

#
# install bash
#

$(STAGEFILES_DIR)/.bash_installed: $(STAGEFILES_DIR)/.bash_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(BASH_BUILD_DIR) install
	$(CP) $(BASH_DIR)/profile $(TARGET_ROOT)/etc/profile
	$(LN) -sfn bash $(TARGET_ROOT)/bin/sh
	$(TOUCH) $(STAGEFILES_DIR)/.bash_installed

$(FILELIST_DIR)/bash.lst: $(STAGEFILES_DIR)/.bash_installed
	$(TOUCH) $(FILELIST_DIR)/bash.lst

.PHONY: clean-bash distclean-bash

clean-bash:
	-$(RM) -rf $(BASH_BUILD_DIR)
	-$(RM) -rf $(BASH_DIR)

distclean-bash:
	-$(RM) -f $(STAGEFILES_DIR)/.bash_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.bash_patched
	-$(RM) -f $(STAGEFILES_DIR)/.bash_configured
	-$(RM) -f $(STAGEFILES_DIR)/.bash_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.bash_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BASH_DLFILE)
endif
