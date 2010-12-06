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
BOOST_DEPS = $(BASE_BUILD_STAGEFILE)

BOOST_VERSION := 1_44_0
BOOST_PATCHES_DIR := $(PATCHES_DIR)/boost/$(BOOST_VERSION)

BOOST_FILE := boost_$(BOOST_VERSION).tar.bz2
BOOST_DLFILE := $(DOWNLOAD_DIR)/$(BOOST_FILE)
BOOST_URL := http://downloads.sourceforge.net/boost/$(BOOST_FILE)
BOOST_DIR := $(BUILD_DIR)/boost_$(BOOST_VERSION)

BOOST_INSTALLED = $(STAGEFILES_DIR)/.boost_installed

PACKS_RULES_$(CONFIG_BOOST) += $(BOOST_INSTALLED)
FILE_LISTS_$(CONFIG_BOOST) += boost.lst

CLEAN_RULES += clean-boost
DISTCLEAN_RULES += distclean-boost

define cp_boost_lst
$(PREFIX_BIN)/$(UCLIBC_STRIP) $(TARGET_ROOT)/usr/lib/libboost_$(1)*.so*; \
$(CP) $(TARGET_ROOT)/usr/lib/libboost_$(1)*.so* $(BOOST_DIR)/rootfs-file-list;
endef

#
# download boost
#

$(BOOST_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(BOOST_DLFILE) ] ; then \
	$(WGET) $(BOOST_URL) -O $(BOOST_DLFILE) ; \
	fi );
	$(TOUCH) $(BOOST_DLFILE)

#
# unpack boost
#

$(STAGEFILES_DIR)/.boost_unpacked: $(BOOST_DLFILE) \
                                           $(wildcard $(BOOST_PATCHES_DIR)/*.patch) \
                                           $$(BOOST_DEPS)
	-$(RM) -rf $(BOOST_DIR)
	$(BZCAT) $(BOOST_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.boost_unpacked

#
# patch boost
#

$(STAGEFILES_DIR)/.boost_patched: $(STAGEFILES_DIR)/.boost_unpacked
	$(call patch_package, $(BOOST_DIR), $(BOOST_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.boost_patched

#
# configure boost
#

$(STAGEFILES_DIR)/.boost_configured: $(STAGEFILES_DIR)/.boost_patched
	($(CD) $(BOOST_DIR) ; $(UCLIBC_ENV) \
		echo "using gcc : : $(UCLIBC_CC) : <cflags>$(UCLIBC_CFLAGS) <cxxflags>$(UCLIBC_CXXFLAGS) <linkflags>-L$(TARGET_ROOT)/usr/lib -L$(TARGET_ROOT)/lib ;" > tools/build/v2/site-config.jam ; \
	)
	$(TOUCH) $(STAGEFILES_DIR)/.boost_configured

#
# compile boost
#

$(STAGEFILES_DIR)/.boost_compiled: $(STAGEFILES_DIR)/.boost_configured
	($(CD) $(BOOST_DIR) ; $(UCLIBC_ENV) \
		bjam \
			'-sBUILD=release <optimization>space <inlining>on <debug-symbols>off' \
			--toolset=gcc --build-type=minimal --layout=system \
			--disable-long-double \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			$(if $(CONFIG_BOOST-date_time),--with-date_time) \
			$(if $(CONFIG_BOOST-filesystem),--with-filesystem) \
			$(if $(CONFIG_BOOST-graph),--with-graph) \
			$(if $(CONFIG_BOOST-graph_parallel),--with-graph_parallel) \
			$(if $(CONFIG_ZLIB),$(if $(CONFIG_BOOST-iostreams),--with-iostreams -sNO_BZIP2=1 -sZLIB_INCLUDE=$(TARGET_ROOT)/usr/include -sZLIB_LIBPATH=$(TARGET_ROOT)/usr/lib)) \
			$(if $(CONFIG_BOOST-math),--with-math) \
			$(if $(CONFIG_BOOST-mpi),--with-mpi) \
			$(if $(CONFIG_BOOST-program_options),--with-program_options) \
			$(if $(CONFIG_BOOST-python),--with-python) \
			$(if $(CONFIG_BOOST-random),--with-random) \
			$(if $(CONFIG_BOOST-regex),--with-regex) \
			$(if $(CONFIG_BOOST-serialization),--with-serialization) \
			$(if $(CONFIG_BOOST-signals),--with-signals) \
			$(if $(CONFIG_BOOST-system),--with-system) \
			$(if $(CONFIG_BOOST-test),--with-test) \
			$(if $(CONFIG_BOOST-thread),--with-thread) \
			$(if $(CONFIG_BOOST-wave),--with-wave) \
	)
	$(TOUCH) $(STAGEFILES_DIR)/.boost_compiled

#
# install boost
#

$(STAGEFILES_DIR)/.boost_installed: $(STAGEFILES_DIR)/.boost_compiled
	($(CD) $(BOOST_DIR) ; $(UCLIBC_ENV) \
		bjam \
			'-sBUILD=release <optimization>space <inlining>on <debug-symbols>off' \
			--toolset=gcc --build-type=minimal --layout=system \
			--disable-long-double \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			$(if $(CONFIG_BOOST-date_time),--with-date_time) \
			$(if $(CONFIG_BOOST-filesystem),--with-filesystem) \
			$(if $(CONFIG_BOOST-graph),--with-graph) \
			$(if $(CONFIG_BOOST-graph_parallel),--with-graph_parallel) \
			$(if $(CONFIG_ZLIB),$(if $(CONFIG_BOOST-iostreams),--with-iostreams -sNO_BZIP2=1 -sZLIB_INCLUDE=$(TARGET_ROOT)/usr/include -sZLIB_LIBPATH=$(TARGET_ROOT)/usr/lib)) \
			$(if $(CONFIG_BOOST-math),--with-math) \
			$(if $(CONFIG_BOOST-mpi),--with-mpi) \
			$(if $(CONFIG_BOOST-program_options),--with-program_options) \
			$(if $(CONFIG_BOOST-python),--with-python) \
			$(if $(CONFIG_BOOST-random),--with-random) \
			$(if $(CONFIG_BOOST-regex),--with-regex) \
			$(if $(CONFIG_BOOST-serialization),--with-serialization) \
			$(if $(CONFIG_BOOST-signals),--with-signals) \
			$(if $(CONFIG_BOOST-system),--with-system) \
			$(if $(CONFIG_BOOST-test),--with-test) \
			$(if $(CONFIG_BOOST-thread),--with-thread) \
			$(if $(CONFIG_BOOST-wave),--with-wave) \
			install; \
		$(RM) -rf $(BOOST_DIR)/rootfs-file-list; \
		$(MKDIR) -p $(BOOST_DIR)/rootfs-file-list; \
		$(if $(CONFIG_BOOST-date_time),$(call cp_boost_lst,date_time)) \
		$(if $(CONFIG_BOOST-filesystem),$(call cp_boost_lst,filesystem)) \
		$(if $(CONFIG_BOOST-graph),$(call cp_boost_lst,graph)) \
		$(if $(CONFIG_BOOST-graph_parallel),$(call cp_boost_lst,graph_parallel)) \
		$(if $(CONFIG_ZLIB),$(if $(CONFIG_BOOST-iostreams),$(call cp_boost_lst,iostreams))) \
		$(if $(CONFIG_BOOST-math),$(call cp_boost_lst,math)) \
		$(if $(CONFIG_BOOST-mpi),$(call cp_boost_lst,mpi)) \
		$(if $(CONFIG_BOOST-program_options),$(call cp_boost_lst,program_options)) \
		$(if $(CONFIG_BOOST-python),$(call cp_boost_lst,python)) \
		$(if $(CONFIG_BOOST-random),$(call cp_boost_lst,random)) \
		$(if $(CONFIG_BOOST-regex),$(call cp_boost_lst,regex)) \
		$(if $(CONFIG_BOOST-serialization),$(call cp_boost_lst,serialization)) \
		$(if $(CONFIG_BOOST-signals),$(call cp_boost_lst,signals)) \
		$(if $(CONFIG_BOOST-system),$(call cp_boost_lst,system)) \
		$(if $(CONFIG_BOOST-test),$(call cp_boost_lst,unit_test_framework)) \
		$(if $(CONFIG_BOOST-test),$(call cp_boost_lst,prg_exec_monitor)) \
		$(if $(CONFIG_BOOST-thread),$(call cp_boost_lst,thread)) \
		$(if $(CONFIG_BOOST-wave),$(call cp_boost_lst,wave)) \
	)
	$(TOUCH) $(STAGEFILES_DIR)/.boost_installed


$(FILELIST_DIR)/boost.lst: $(STAGEFILES_DIR)/.boost_installed
$(eval $(call gen_copy_file_lst,$(BOOST_DIR)/rootfs-file-list,usr/lib,0,0,boost.lst,check))

.PHONY: clean-boost distclean-boost

clean-boost:
	-$(RM) -rf $(BOOST_DIR)

#
# clean everthing else
#

distclean-boost:
	-$(RM) -f $(STAGEFILES_DIR)/.boost_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.boost_patched
	-$(RM) -f $(STAGEFILES_DIR)/.boost_configured
	-$(RM) -f $(STAGEFILES_DIR)/.boost_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.boost_installed
	-$(RM) -f $(FILELIST_DIR)/boost.lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BOOST_DLFILE)
endif
