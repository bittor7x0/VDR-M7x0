# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id: siemens-gpl-src.mk 374 2008-09-01 18:22:42Z andreas $
#


# We are using m750 src for both system types as there are nearly no difference
# the kernel has some very small patches attached.
# As the base dir in archive is named gigaset_m740av_gplsw shows the source is
# should be the same.

SIEMENS-GPL-SRC_FILE := siemens-m750-gpl-src.tar.bz2
SIEMENS-GPL-SRC_DLFILE = $(DOWNLOAD_DIR)/$(SIEMENS-GPL-SRC_FILE)
SIEMENS-BUILD-ENV_FILE := siemens-build_env.tar.gz
SIEMENS-BUILD-ENV_DLFILE = $(DOWNLOAD_DIR)/$(SIEMENS-BUILD-ENV_FILE)

SIEMENS-GPL-SRC_URL := http://gigaset.siemens.com/repository/1222/122244/gigaset_m750_gplsw_1_4_3_tar.bz2
SIEMENS-BUILD-ENV_URL := http://gigaset.siemens.com/repository/1237/123736/wavebox_build_env.tar.gz
# As the extracted archive is very big (around 1GB) and
# we need only a very small part of it we don't extracted fully
# BTW: This thing has wrong naming it should called siemens-gpl-trash ...
SIEMENS-GPL-SRC-TAR_BASE_DIR = gigaset_m740av_gplsw
SIEMENS-BUILD-ENV_TAR_BASE_DIR = wavebox_build_env

DISTCLEAN_RULES += distclean-siemens-gpl-src

#
# download siemens-gpl-src
#

$(SIEMENS-GPL-SRC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SIEMENS-GPL-SRC_DLFILE) ] ; then \
	$(WGET) $(SIEMENS-GPL-SRC_URL) -O $(SIEMENS-GPL-SRC_DLFILE) ; \
	fi );
	$(TOUCH) $(SIEMENS-GPL-SRC_DLFILE)

$(SIEMENS-BUILD-ENV_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SIEMENS-BUILD-ENV_DLFILE) ] ; then \
	$(WGET) $(SIEMENS-BUILD-ENV_URL) -O $(SIEMENS-BUILD-ENV_DLFILE) ; \
	fi );
	$(TOUCH) $(SIEMENS-BUILD-ENV_DLFILE)


distclean-siemens-gpl-src:
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SIEMENS-GPL-SRC_DLFILE)
endif
