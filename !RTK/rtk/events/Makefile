# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

CPPFLAGS = -Irtk_build:
CXXFLAGS = -mthrowback -munixlib -mpoke-function-name -Wall -W -Wno-unused -O2

include fixdeps:Rules/make

OBJ = event.o \
 wimp.o \
 null_reason.o \
 close_window.o \
 entering_window.o \
 leaving_window.o \
 mouse_click.o \
 user_drag_box.o \
 key_pressed.o \
 menu_selection.o \
 message.o \
 quit.o \
 menuwarning.o \
 menusdeleted.o \
 datasave.o \
 datasaveack.o \
 dataload.o \
 dataloadack.o \
 dataopen.o \
 ramfetch.o \
 ramtransmit.o \
 help_request.o \
 reopen_menu.o \
 save_to_file.o \
 save_to_app.o \
 discard.o \
 save.o \
 loaded.o \
 saved.o \
 redirection.o

all: timestamp

timestamp: $(OBJ)
	touch timestamp

include $(OBJ:.o=.d)
