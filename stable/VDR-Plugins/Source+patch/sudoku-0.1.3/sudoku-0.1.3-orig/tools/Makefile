#
# Sudoku: A plugin for the Video Disk Recorder
#
# See the README file for copyright information and how to reach the author.
#
# $Id: Makefile 28 2006-04-25 00:09:14Z tom $

# Define STATIC_LINK=1 to force static linking
#STATIC_LINK = 1

# Define WITH_TEST=1 to include test procedures
#WITH_TEST = 1

PROGRAM = sudoku_generator

SRCS = ../puzzle.cpp ../generator.cpp ../solver.cpp ../backtrack.cpp

VERSION = $(shell sed -ne '/static .* VERSION *=/s/^.*"\(.*\)".*$$/\1/p' \
                      ../sudoku.cpp)

CXX      ?= g++
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual
DEFINES  += -D_GNU_SOURCE -DVERSION=\"$(VERSION)\"

ifdef STATIC_LINK
    CXXFLAGS += -static
endif

ifdef WITH_TEST
    DEFINES += -DWITH_TEST
endif

### Targets:

all: $(PROGRAM)

$(PROGRAM): $(PROGRAM).cpp $(SRCS) $(SRCS:%.cpp=%.h)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -o $@ $(PROGRAM).cpp $(SRCS)

clean:
	@-rm -f $(PROGRAM) core* *~
