# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 VDR-NG-EM Project
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

ifeq ($(strip $(shell which java)),)
	$(error dependency error: YUI Compressor needs Java Runtime Environment)
endif

YUI_COMPRESSOR_HOSTVERSION := 2.4.8
YUI_COMPRESSOR_JAR := $(HOSTUTILS_PREFIX_BIN)/yuicompressor-$(YUI_COMPRESSOR_HOSTVERSION).jar
YUI_COMPRESSOR_HOSTFILE := yuicompressor-$(YUI_COMPRESSOR_HOSTVERSION).zip
YUI_COMPRESSOR_HOSTDLFILE := $(DOWNLOAD_DIR)/$(YUI_COMPRESSOR_HOSTFILE)

YUI_COMPRESSOR_HOSTURL := https://github.com/yui/yuicompressor/releases/download/v$(YUI_COMPRESSOR_HOSTVERSION)/$(YUI_COMPRESSOR_HOSTFILE)
YUI_COMPRESSOR_HOSTDIR := $(HOSTUTILS_BUILD_DIR)/yuicompressor-$(YUI_COMPRESSOR_HOSTVERSION)

CLEAN_RULES += clean-yui_compressor-host
DISTCLEAN_RULES += distclean-yui_compressor-host

#
# download yui_compressor
#

$(YUI_COMPRESSOR_HOSTDLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(YUI_COMPRESSOR_HOSTDLFILE) ] ; then \
	$(WGET) $(YUI_COMPRESSOR_HOSTURL) -O $(YUI_COMPRESSOR_HOSTDLFILE) ; \
	fi );
	$(TOUCH) $(YUI_COMPRESSOR_HOSTDLFILE)

#
# unpack yui_compressor
#

$(STAGEFILES_DIR)/.yui_compressor_host_unpacked: $(YUI_COMPRESSOR_HOSTDLFILE)
	-$(RM) -rf $(YUI_COMPRESSOR_HOSTDIR)
	$(MKDIR) -p $(YUI_COMPRESSOR_HOSTDIR)
	(cd $(YUI_COMPRESSOR_HOSTDIR) ; $(UNZIP) -x $(YUI_COMPRESSOR_HOSTDLFILE))
	$(TOUCH) $(STAGEFILES_DIR)/.yui_compressor_host_unpacked

#
# install yui_compressor
#

$(YUI_COMPRESSOR_JAR): $(STAGEFILES_DIR)/.yui_compressor_host_unpacked
	$(CP) $(YUI_COMPRESSOR_HOSTDIR)/yuicompressor-$(YUI_COMPRESSOR_HOSTVERSION).jar $(YUI_COMPRESSOR_JAR)

.PHONY: clean-yui_compressor-host distclean-yui_compressor-host

clean-yui_compressor-host:
	-$(RM) -rf $(YUI_COMPRESSOR_HOSTDIR)

distclean-yui_compressor-host:
	-$(RM) -f $(STAGEFILES_DIR)/.yui_compressor_host_unpacked
	-$(RM) -f $(YUI_COMPRESSOR_JAR)
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(YUI_COMPRESSOR_HOSTDLFILE)
endif
