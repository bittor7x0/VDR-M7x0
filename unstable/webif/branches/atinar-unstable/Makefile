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
-include Makefile.conf

# klone version to use
KLONE_VERSION ?= 2.1.1

# klone configure arguments
KLONE_CONF_ARGS = --disable-lib=openssl --disable-lib=z --disable_cxx
KLONE_CONF_ARGS += --enable_fs --enable_hooks
# KLONE_CONF_ARGS += --enable_debug --enable_libu_debug
# KLONE_CONF_ARGS += --prefix=$(DESTDIR) --libs=$(DESTDIR) --disable_libu_mini --disable_shared

# exclude svn control files
KLONE_IMPORT_ARGS = -x */.svn/* -x */logos/*

# patch klone to include partial content responses
KLONE_TARGET_PATCH_FILE = $(CURDIR)/klone-core-2.1.1-atinar.patch 
KLONE_HOST_PATCH_FILE= $(KLONE_TARGET_PATCH_FILE)

# cross-compiling (see build/makl/tc/* for the list of supported platforms)
# MAKL_PLATFORM ?=

# or provide custom toolchain file
KLONE_CUSTOM_TC ?= $(KLONE_APP_TOP)/linux-mips-uclibc.tc

# helper libs
SUBDIR ?= $(KLONE_APP_TOP)/webapp/inc/

# web application directory (defaults to webapp/ in the current dir)
# WEBAPP_DIR ?=

# additional CFLAGS, LDFLAGS and LDADD (optional, use absolute paths for LDADD)
#DEBUG  = -g -ggdb -DDEBUG
COMMON_CFLAGS = -I$(KLONE_SRC) $(UCLIBC_CFLAGS) -D_GNU_SOURCE $(DEBUG)
WEBAPP_CFLAGS ?= $(COMMON_CFLAGS) -I$(KLONE_APP_TOP) -I$(SUBDIR)
WEBAPP_LDADD ?= ${SUBDIR}basic_auth.o ${SUBDIR}browse.o $(SUBDIR)channels.o ${SUBDIR}conf.o $(SUBDIR)epg.o $(SUBDIR)hooks.o ${SUBDIR}i18n.o $(SUBDIR)misc.o $(SUBDIR)partitions.o $(SUBDIR)recording.o $(SUBDIR)recordings.o $(SUBDIR)searches.o $(SUBDIR)svdrp_comm.o $(SUBDIR)timers.o
WEBAPP_LDFLAGS ?=

-include build/mk/klone-app.mk

