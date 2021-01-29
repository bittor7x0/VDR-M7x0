# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2021 VDR-NG-EM Project
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

ifeq ($(strip $(CONFIG_ROOTFS_DIR)),)
  ROOTFS_DIR = $(TOP_DIR)/rootfs_dir_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)
else
  ROOTFS_DIR := $(abspath $(CONFIG_ROOTFS_DIR))
endif
ROOTFS_FILE_TABLE := $(STAGEFILES_DIR)/file_tab$(subst /,.,$(subst $(TOP_DIR),,$(ROOTFS_DIR))).lst
ROOTFS_FILE_COPY := $(STAGEFILES_DIR)/file_copy$(subst /,.,$(subst $(TOP_DIR),,$(ROOTFS_DIR))).lst
FQ_FILE_LISTS = $(addprefix $(FILELIST_DIR)/, $(FILE_LISTS))
ROOTFS_DIR_DEPS = $(PACKS_BUILD_STAGEFILE) $$(FQ_FILE_LISTS) \
   $(COPY_LISTS_BIN) $(TOP_DIR)/.config
ifeq ($(HOST_CYGWIN),y)
	ROOTFS_DIR_DEPS += $(CRAMFSCK_BIN) $(CRAMFSSWAP_BIN) $(MKSQUASHFS_BIN) $(RSAENCODE_BIN)
	CYGWIN_DIR_TMP_REL := cygwin_dir_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)
	CYGWIN_DIR_TMP_ABS := $(TOP_DIR)/$(CYGWIN_DIR_TMP_REL)
endif

ifndef CONFIG_EXT2_ROOTFS_SIZE
  EXT2_ROOTFS_SIZE := 65536
else
  EXT2_ROOTFS_SIZE := CONFIG_EXT2_ROOTFS_SIZE
endif

ifeq ($(strip $(notdir $(CONFIG_EXT2_ROOTFS_IMG))),)
  EXT2_ROOTFS_IMG := rootfs-ext2_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).img
else
  EXT2_ROOTFS_IMG :=$(notdir $(CONFIG_EXT2_ROOTFS_IMG))
endif

ifeq ($(strip $(notdir $(CONFIG_CRAM_ROOTFS_IMG))),)
  CRAM_ROOTFS_IMG = rootfs-cram_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).img
else
  CRAM_ROOTFS_IMG =$(notdir $(CONFIG_CRAM_ROOTFS_IMG))
endif

ifeq ($(strip $(notdir $(CONFIG_SQUASH_ROOTFS_IMG))),)
  SQUASH_ROOTFS_IMG = rootfs-squash_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).img
else
  SQUASH_ROOTFS_IMG =$(notdir $(CONFIG_SQUASH_ROOTFS_IMG))
endif

POST_RULES_$(CONFIG_GENERATE_ROOTFS_DIR) += $(ROOTFS_FILE_TABLE)
POST_RULES_$(CONFIG_GENERATE_EXT2_ROOTFS_IMAGE) += $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
POST_RULES_$(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE) += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
POST_RULES_$(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE) += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)

DISTCLEAN_RULES += distclean-generate-rootfs

define awk_rootfs_lst_trans_prg
$$1 ~ /^\/?etc\/passwd/ || \
$$1 ~ /^\/?etc\/shadow/ || \
$$1 ~ /^\/?etc\/group/ || \
$$1 ~ /^\/?etc\/inittab/ || \
$$1 ~ /^\/?etc\/rc.mini/ || \
$$1 ~ /^\/?etc\/rc.debug/ || \
$$1 ~ /^\/?etc\/systemtype/ || \
$$1 ~ /^\/?etc\/hostname/ || \
$$1 ~ /^\/?etc\/.*\.conf/ || \
$$1 ~ /^\/?etc\/usbautomounter$$/ || \
$$1 ~ /^\/?etc\/mediatomb$$/ || \
$$1 ~ /^\/?etc\/mediatomb\/config.xml/ || \
$$1 ~ /^\/?etc\/mediatomb\/mappings.xml/ || \
$$1 ~ /^\/?etc\/mediatomb\/mediatomb.db/ || \
$$1 ~ /^\/?etc\/samba$$/ || \
$$1 ~ /^\/?etc\/webif$$/ || \
$$1 ~ /^\/?etc\/ssmtp$$/ || \
$$1 ~ /^\/?etc\/vdr$$/ || \
$$1 ~ /^\/?etc\/vdr\/runvdr/ || \
$$1 ~ /^\/?etc\/vdr\/plugins$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/epgsearch$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/epgfixer$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/filebrowser$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/scheduler$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/streamdev$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/tvscraper$$/ || \
$$1 ~ /^\/?etc\/vdr\/plugins\/xvdr$$/
endef

AWK_LST_TRANS_PRG_COPY := '$$1 !~ /^\/?etc\// || $(call awk_rootfs_lst_trans_prg) \
	{ print }'

AWK_LST_TRANS_PRG := '$$1 !~ /\#/ && $$3 !~ /l/ && $$3 !~ /s/ \
                   && ($$1 !~ /^\/?etc\// || $(call awk_rootfs_lst_trans_prg)) \
                   && $$1 !~ /^\/?root/ && $$1 !~ /^\/?home/ \
   { file="/"$$1 ; gsub("//","/",file) ; \
    print file, $$3, $$4, $$5, $$6, $$7, $$8, $$9, $$10, $$11}'

# Strip modules (based in rstrip.sh from OpenWrt.org)
STRIP_KMOD = "$(PREFIX_BIN)/$(UCLIBC_TARGET)-strip --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.MIPS.abiflags --remove-section=.note.gnu.build-id --remove-section=.note.GNU-stack --remove-section=.gnu.attributes --remove-section=.reginfo"
define find_modparams
	$(PREFIX_BIN)/$(UCLIBC_NM) "$1" | $(AWK) ' \
	BEGIN { \
		FS=" " \
	} \
	($$3 ~ /^__module_parm_/) && ($$3 !~ /^__module_parm_desc/) { \
		gsub(/__module_parm_/, "", $$3) ; \
		printf "-K " $$3 " " \
	} \
	($$2 ~ /r/) && ($$3 ~ /__param_/) { \
		gsub(/__param_/, "", $$3) ; \
		printf "-K " $$3 " " \
	} \
	'
endef

$(ROOTFS_FILE_TABLE): $(ROOTFS_DIR_DEPS)
	-$(RM) -rf $(ROOTFS_DIR)
	-$(RM) -f $(ROOTFS_FILE_TABLE)
	$(MKDIR) -p $(ROOTFS_DIR)
	($(FIND) $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/lib/modules -type f -a -exec file {} \; | \
		$(SED) -n -e 's/^\(.*\):.*ELF.*\(executable\|relocatable\|shared object\).*,.* stripped/\1:\2/p' | \
		( \
			IFS=":" ; \
			while read F S; do \
				$(ECHO) "strip: $$F:$$S" ; \
				[ "$${S}" = "relocatable" ] && { \
					eval "$(STRIP_KMOD) -w -x -K '__param*' -K '__mod*' $$($(call find_modparams,"$$F"))$$F" ; \
				} \
			done ; \
			true ; \
		) \
	)
	$(CAT) $(FQ_FILE_LISTS) | $(AWK) $(AWK_LST_TRANS_PRG_COPY) > \
		$(ROOTFS_FILE_COPY)
	$(COPY_LISTS_BIN) -s '$(ROOTFS_DIR)' '$(TARGET_ROOT)' \
		'$(PREFIX_BIN)/$(UCLIBC_STRIP)' $(ROOTFS_FILE_COPY)
	$(SED) -i -e "s,^export SYSTEMTYPE=.*,export SYSTEMTYPE=`$(CAT) $(ROOTFS_DIR)/etc/systemtype`,g" $(ROOTFS_DIR)/etc/rc.mini
	$(call set_lang, $(ROOTFS_DIR))
	$(call fix_fw_conf_files, $(ROOTFS_DIR))
ifeq ($(HOST_CYGWIN),y)
	-$(RM) -rf $(CYGWIN_DIR_TMP_ABS)
	$(MKDIR) -p $(CYGWIN_DIR_TMP_ABS)
	$(CP) $(HOSTUTILS_PREFIX_BIN)/cramfsck.exe $(CYGWIN_DIR_TMP_ABS)/cramfsck.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/cramfsswap.exe $(CYGWIN_DIR_TMP_ABS)/cramfsswap.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/gen_ofi.exe $(CYGWIN_DIR_TMP_ABS)/gen_ofi.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/gen_wsw.exe $(CYGWIN_DIR_TMP_ABS)/gen_wsw.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/mips-linux-uclibc-objcopy.exe $(CYGWIN_DIR_TMP_ABS)/mips-linux-uclibc-objcopy.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/mksquashfs.exe $(CYGWIN_DIR_TMP_ABS)/mksquashfs.exe
	$(CP) $(HOSTUTILS_PREFIX_BIN)/rsaencode.exe $(CYGWIN_DIR_TMP_ABS)/rsaencode.exe
	$(CP) $(shell which cat.exe) $(CYGWIN_DIR_TMP_ABS)/cat.exe
	$(CP) $(shell which dd.exe) $(CYGWIN_DIR_TMP_ABS)/dd.exe
	$(CP) $(shell which gzip.exe) $(CYGWIN_DIR_TMP_ABS)/gzip.exe
	$(CP) $(shell which md5sum.exe) $(CYGWIN_DIR_TMP_ABS)/md5sum.exe	
	$(CP) $(shell which sed.exe) $(CYGWIN_DIR_TMP_ABS)/sed.exe
	$(CP) $(shell which sha1sum.exe) $(CYGWIN_DIR_TMP_ABS)/sha1sum.exe
	$(CP) $(shell which tar.exe) $(CYGWIN_DIR_TMP_ABS)/tar.exe
	$(CP) $(shell which unzip.exe) $(CYGWIN_DIR_TMP_ABS)/unzip.exe
	for exe_file in `$(FIND) $(CYGWIN_DIR_TMP_ABS) -type f -iname '*.exe' | $(SORT)` ; do \
		for exe_dep in `ldd.exe $$exe_file | grep.exe --invert-match Windows | cut.exe -d " " -f3` ; do \
			[ ! -f $(CYGWIN_DIR_TMP_ABS)/$$(basename $$exe_dep) ] && \
				$(CP) $$exe_dep $(CYGWIN_DIR_TMP_ABS)/$$(basename $$exe_dep) && \
				strip.exe $(CYGWIN_DIR_TMP_ABS)/$$(basename $$exe_dep) ; \
		done ; \
		strip.exe $$exe_file ; \
	done
	-$(RM) -f $(TOP_DIR)/fw-builder-cygwin-bin.tar.bz2
	tar jcvf $(TOP_DIR)/fw-builder-cygwin-bin.tar.bz2 \
		--directory=$(TOP_DIR) $(CYGWIN_DIR_TMP_REL)/
endif
	$(CAT) $(FQ_FILE_LISTS) | $(AWK) $(AWK_LST_TRANS_PRG) > \
		$(ROOTFS_FILE_TABLE)

$(TOP_DIR)/$(EXT2_ROOTFS_IMG): $$(GENEXT2FS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
	$(GENEXT2FS_BIN) -D $(ROOTFS_FILE_TABLE) -d $(ROOTFS_DIR) \
		-b $(EXT2_ROOTFS_SIZE) $(TOP_DIR)/$(EXT2_ROOTFS_IMG)

$(TOP_DIR)/$(SQUASH_ROOTFS_IMG): $$(MKSQUASHFS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
	$(MKSQUASHFS_BIN) $(ROOTFS_DIR) $(TOP_DIR)/$(SQUASH_ROOTFS_IMG) \
		-all-root -be -noappend -noI -no-fragments -d $(ROOTFS_FILE_TABLE)
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
		$(ECHO) Error: SquashFS image too big!; \
		$(ECHO) "       You can use SquashFS image with LZMA compression setting in .config:"; \
		$(ECHO) "          CONFIG_SQUASHFS_LZMA = y"; \
		$(ECHO) "       and running this command before you recompile with 'make':"; \
		$(ECHO) "          make distclean-squashfs-host distclean-siemens-linux-kernel"; \
	    	$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG); \
	    	exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
		$(ECHO) Error: SquashFS image too big!; \
		$(ECHO) "       You can use SquashFS image with LZMA compression setting in .config:"; \
		$(ECHO) "          CONFIG_SQUASHFS_LZMA = y"; \
		$(ECHO) "       and running this command before you recompile with 'make':"; \
		$(ECHO) "          make distclean-squashfs-host distclean-siemens-linux-kernel"; \
	    	$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG); \
	    	exit 1; \
	fi
endif

ifeq ($(CONFIG_CRAMFS_NEEDS_SWAP),y)

$(TOP_DIR)/$(CRAM_ROOTFS_IMG): $$(MKCRAMFS_BIN) $$(CRAMFSSWAP_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	$(MKCRAMFS_BIN) -D $(ROOTFS_FILE_TABLE) $(ROOTFS_DIR) \
		$(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	$(CRAMFSSWAP_BIN) $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host \
                     $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
		$(ECHO) Error: CramFS image too big!; \
		$(ECHO) "       You can use SquashFS image setting in .config:"; \
		$(ECHO) "          CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE = y"; \
		$(ECHO) "       and disabling CramFS image:"; \
		$(ECHO) "          # CONFIG_CRAM_ROOTFS_IMG is not set"; \
	    	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
	    	exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
		$(ECHO) Error: CramFS image too big!; \
		$(ECHO) "       You can use SquashFS image setting in .config:"; \
		$(ECHO) "          CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE = y"; \
		$(ECHO) "       and disabling CramFS image:"; \
		$(ECHO) "          # CONFIG_CRAM_ROOTFS_IMG is not set"; \
	    	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
	    	exit 1; \
	fi
endif
	
else

$(TOP_DIR)/$(CRAM_ROOTFS_IMG): $$(MKCRAMFS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	$(MKCRAMFS_BIN) -D $(ROOTFS_FILE_TABLE) $(ROOTFS_DIR) \
		$(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
			-gt $(CRAMFS_MAX_IMGSIZE); then \
		$(ECHO) Error: CramFS image too big!; \
		$(ECHO) "       You can use SquashFS image setting in .config:"; \
		$(ECHO) "          CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE = y"; \
		$(ECHO) "       and disabling CramFS image:"; \
		$(ECHO) "          # CONFIG_CRAM_ROOTFS_IMG is not set"; \
		$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
		exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
			-gt $(CRAMFS_MAX_IMGSIZE); then \
		$(ECHO) Error: CramFS image too big!; \
		$(ECHO) "       You can use SquashFS image setting in .config:"; \
		$(ECHO) "          CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE = y"; \
		$(ECHO) "       and disabling CramFS image:"; \
		$(ECHO) "          # CONFIG_CRAM_ROOTFS_IMG is not set"; \
		$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
		exit 1; \
	fi
endif

endif

.PHONY: distclean-generate-rootfs
distclean-generate-rootfs:
	-$(RM) -f $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(ROOTFS_FILE_COPY)
	-$(RM) -rf $(ROOTFS_DIR)
	-$(RM) -f $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
