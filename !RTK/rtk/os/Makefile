# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

CPPFLAGS = -Irtk_build:
CXXFLAGS = -mthrowback -munixlib -mpoke-function-name -Wall -W -Wno-unused -O2

include fixdeps:Rules/make

OBJ = exception.o \
 os.o \
 font.o \
 wimp.o \
 colourtrans.o \
 messagetrans.o \
 dragasprite.o

all: timestamp

timestamp: $(OBJ)
	touch timestamp

include $(OBJ:.o=.d)
