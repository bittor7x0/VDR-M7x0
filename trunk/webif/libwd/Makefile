-include common.mk
-include ../vars.mk

# name of the library
LIB = wd

# list of source files
SRCS = propfind.c

# include ${KLONE_CUSTOM_TC}
#DEBUG  = -g -ggdb -DDEBUG

# compilation flags
CFLAGS += $(COMMON_CFLAGS) $(KLONE_CFLAGS) -I$(KLONE_SRC)/libu/include/

-include lib.mk

clean:
	rm -f *.o libwd.a
