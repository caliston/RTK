# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

include ../../Rules.make

OBJ = event.o \
 wimp.o \
 null_reason.o \
 message.o \
 quit.o \
 reopen_menu.o

all: $(OBJ)

include $(OBJ:.o=.d)
