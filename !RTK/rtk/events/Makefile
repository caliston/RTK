# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

include ../../Rules.make

OBJ = event.o \
 wimp.o \
 null_reason.o \
 close_window.o \
 entering_window.o \
 leaving_window.o \
 mouse_click.o \
 key_pressed.o \
 menu_selection.o \
 message.o \
 quit.o \
 help_request.o \
 reopen_menu.o \
 redirection.o

all: $(OBJ)

include $(OBJ:.o=.d)
