# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

include ../../Rules.make

OBJ = version.o \
 refcount.o \
 divider.o

all: timestamp

timestamp: $(OBJ)
	touch timestamp

include $(OBJ:.o=.d)
