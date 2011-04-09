# Variables to be defined
#
#   KLONE_VERSION  KLone version to use (will be downloaded from koanlogic.com)
#
#   SUBDIR          (optional) list of additional directories to build on make
#
#   WEBAPP_CFLAGS   (optional) additional CFLAGS
#   WEBAPP_LDFLAGS  (optional) additional LDFLAGS
#   WEBAPP_LDADD    (optional) additional object files and/or static libs (.a)
#                              (requires absolute paths)
#
# Predefined variables:
#
#  KLONE_APP_TOP    top-dir directory (where this Makefile is stored)
#  KLONE_SRC        location of klone source tree
#
export FETCH_CMD = wget

KLONE_VERSION = 2.3.0

# klone configure arguments
KLONE_CONF_ARGS = --disable-lib=openssl --disable-lib=z --disable_cxx
KLONE_CONF_ARGS += --enable_fs --enable_hooks
# KLONE_CONF_ARGS += --enable_debug --enable_libu_debug
# KLONE_CONF_ARGS += --prefix=$(DESTDIR) --libs=$(DESTDIR) --disable_libu_mini --disable_shared

# exclude svn control files
KLONE_IMPORT_ARGS = -x */.svn/*

# patch klone to include partial content responses
KLONE_TARGET_PATCH_FILE = $(CURDIR)/klone-core-${KLONE_VERSION}-atinar.patch 
KLONE_TARGET_PATCH_FILE += $(CURDIR)/klone-core-${KLONE_VERSION}-gcc.patch 
KLONE_HOST_PATCH_FILE= $(KLONE_TARGET_PATCH_FILE)

# cross-compiling (see build/makl/tc/* for the list of supported platforms)
# MAKL_PLATFORM =

# or provide custom toolchain file
# KLONE_CUSTOM_TC = $(CURDIR)/linux-mips-uclibc.tc

# helper libs
SUBDIR = $(CURDIR)/libwi

# web application directory (defaults to webapp/ in the current dir)
WEBAPP_DIR = $(CURDIR)/webapp

# additional CFLAGS, LDFLAGS and LDADD (optional, use absolute paths for LDADD)
# DEBUG  = -g -ggdb -DDEBUG
export COMMON_CFLAGS = -I$(KLONE_SRC) -I$(CURDIR)/include $(UCLIBC_CFLAGS) $(DEBUG) -D_GNU_SOURCE
WEBAPP_CFLAGS = $(COMMON_CFLAGS)
WEBAPP_LDADD = ${CURDIR}/libwi/libwi.a

include klapp.mk

klapp.mk:
	${FETCH_CMD} -O $@ -c http://koanlogic.com/klone/klapp-2.0.0.mk

