# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2014-2024 VDR-NG-EM Project
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LIBC-BENCH_DEPS = $(BASE_BUILD_STAGEFILE)

LIBC-BENCH_VERSION := 20110206
LIBC-BENCH_PATCHES_DIR := $(PATCHES_DIR)/libc-bench/$(LIBC-BENCH_VERSION)

LIBC-BENCH_FILE := libc-bench-$(LIBC-BENCH_VERSION).tar.gz
LIBC-BENCH_DLFILE := $(DOWNLOAD_DIR)/$(LIBC-BENCH_FILE)
LIBC-BENCH_URL := http://www.etalabs.net/releases/$(LIBC-BENCH_FILE)
LIBC-BENCH_DIR := $(BUILD_DIR)/libc-bench-$(LIBC-BENCH_VERSION)

LIBC-BENCH_PACKED = $(STAGEFILES_DIR)/.libc-bench_packed

PACKS_RULES_$(CONFIG_LIBC-BENCH) += $(LIBC-BENCH_PACKED)

CLEAN_RULES += clean-libc-bench
DISTCLEAN_RULES += distclean-libc-bench

#
# download libc-bench
#

$(LIBC-BENCH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIBC-BENCH_DLFILE) ] ; then \
	$(WGET) $(LIBC-BENCH_URL) -O $(LIBC-BENCH_DLFILE) ; \
	fi );
	$(TOUCH) $(LIBC-BENCH_DLFILE)

#
# unpack libc-bench
#

$(STAGEFILES_DIR)/.libc-bench_unpacked: $(LIBC-BENCH_DLFILE) \
                                           $(wildcard $(LIBC-BENCH_PATCHES_DIR)/*.patch) \
                                           $$(LIBC-BENCH_DEPS)
	-$(RM) -rf $(LIBC-BENCH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LIBC-BENCH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.libc-bench_unpacked

#
# patch libc-bench
#

$(STAGEFILES_DIR)/.libc-bench_patched: $(STAGEFILES_DIR)/.libc-bench_unpacked
	$(call patch_package, $(LIBC-BENCH_DIR), $(LIBC-BENCH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.libc-bench_patched

#
# compile libc-bench
#

$(STAGEFILES_DIR)/.libc-bench_compiled: $(STAGEFILES_DIR)/.libc-bench_patched
	$(UCLIBC_ENV) $(MAKE) \
		-C $(LIBC-BENCH_DIR) \
		CC=$(UCLIBC_CC) \
		CFLAGS="$(UCLIBC_CFLAGS)" \
		STRIP="$(UCLIBC_STRIP)"
	$(TOUCH) $(STAGEFILES_DIR)/.libc-bench_compiled

#
# strip libc-bench
#

$(STAGEFILES_DIR)/.libc-bench_stripped: $(STAGEFILES_DIR)/.libc-bench_compiled
	$(UCLIBC_ENV) $(MAKE) strip \
		-C $(LIBC-BENCH_DIR) \
		CC=$(UCLIBC_CC) \
		CFLAGS="$(UCLIBC_CFLAGS)" \
		STRIP="$(UCLIBC_STRIP)"
	$(TOUCH) $(STAGEFILES_DIR)/.libc-bench_stripped

#
# pack libc-bench
#

$(STAGEFILES_DIR)/.libc-bench_packed: $(STAGEFILES_DIR)/.libc-bench_stripped
	-$(RM) -f $(TOP_DIR)/libc-bench$(LIBC-BENCH_VERSION)-m7x0.tar.bz2
	tar jcvf $(TOP_DIR)/libc-bench$(LIBC-BENCH_VERSION)-m7x0.tar.bz2 \
		--directory=$(LIBC-BENCH_DIR) libc-bench
	$(TOUCH) $(STAGEFILES_DIR)/.libc-bench_packed

.PHONY: clean-libc-bench distclean-libc-bench

clean-libc-bench:
	-$(RM) -rf $(LIBC-BENCH_DIR)

#
# clean everthing else
#

distclean-libc-bench:
	-$(RM) -f $(STAGEFILES_DIR)/.libc-bench_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.libc-bench_patched
	-$(RM) -f $(STAGEFILES_DIR)/.libc-bench_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libc-bench_stripped
	-$(RM) -f $(STAGEFILES_DIR)/.libc-bench_packed
	-$(RM) -f $(TOP_DIR)/libc-bench$(LIBC-BENCH_VERSION)-m7x0.tar.bz2
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIBC-BENCH_DLFILE)
endif
