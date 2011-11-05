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
# The project's page is at http://www.open7x0.org
#

ifeq ($(strip $(shell which java)),)
	$(error dependency error: Closure Compiler needs Java Runtime Environment)
endif

CLOSURE_COMPILER_JAR := $(HOSTUTILS_PREFIX_BIN)/closure_compiler.jar

CLOSURE_COMPILER_HOSTVERSION := 20110811
CLOSURE_COMPILER_HOSTFILE := compiler-$(CLOSURE_COMPILER_HOSTVERSION).tar.gz
CLOSURE_COMPILER_HOSTDLFILE := $(DOWNLOAD_DIR)/$(CLOSURE_COMPILER_HOSTFILE)

CLOSURE_COMPILER_HOSTPATCHES_DIR := $(PATCHES_DIR)/closure_compiler/$(CLOSURE_COMPILER_HOSTVERSION)
CLOSURE_COMPILER_HOSTURL := http://closure-compiler.googlecode.com/files/$(CLOSURE_COMPILER_HOSTFILE)
CLOSURE_COMPILER_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/closure_compiler-$(CLOSURE_COMPILER_HOSTVERSION)

CLEAN_RULES += clean-closure_compiler-host
DISTCLEAN_RULES += distclean-closure_compiler-host

#
# download closure_compiler
#

$(CLOSURE_COMPILER_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(CLOSURE_COMPILER_HOSTDLFILE) ] ; then \
	$(WGET) $(CLOSURE_COMPILER_HOSTURL) -O $(CLOSURE_COMPILER_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(CLOSURE_COMPILER_HOSTDLFILE)

#
# unpack closure_compiler
#

$(STAGEFILES_DIR)/.closure_compiler_host_unpacked: $(CLOSURE_COMPILER_HOSTDLFILE)
	-$(RM) -rf $(CLOSURE_COMPILER_HOSTDIR)
	$(MKDIR) -p $(CLOSURE_COMPILER_HOSTDIR)
	$(TAR) -C $(CLOSURE_COMPILER_HOSTDIR) -zf $(CLOSURE_COMPILER_HOSTDLFILE) 
	$(TOUCH) $(STAGEFILES_DIR)/.closure_compiler_host_unpacked

#
# install closure_compiler
#

$(HOSTUTILS_PREFIX_BIN)/closure_compiler.jar: $(STAGEFILES_DIR)/.closure_compiler_host_unpacked
	$(CP) $(CLOSURE_COMPILER_HOSTDIR)/compiler.jar $(HOSTUTILS_PREFIX_BIN)/closure_compiler.jar

.PHONY: clean-closure_compiler-host distclean-closure_compiler-host

clean-closure_compiler-host:
	-$(RM) -rf $(CLOSURE_COMPILER_HOSTDIR)

distclean-closure_compiler-host:
	-$(RM) -f $(STAGEFILES_DIR)/.closure_compiler_host_unpacked
	-$(RM) -f $(HOSTUTILS_PREFIX_BIN)/closure_compiler.jar
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(CLOSURE_COMPILER_HOSTDLFILE)
endif
