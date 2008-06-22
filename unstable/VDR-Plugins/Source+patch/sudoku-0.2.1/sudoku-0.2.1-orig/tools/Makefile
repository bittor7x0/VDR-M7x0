#
# Sudoku: A plug-in for the Video Disk Recorder
#
# Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# $Id: Makefile 106 2007-12-03 23:28:24Z tom $

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
