# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

include ../../Rules.make

OBJ = component.o \
 sizeable_component.o \
 absolute_layout.o \
 grid_layout.o \
 row_layout.o \
 column_layout.o \
 label.o \
 icon.o \
 action_button.o \
 default_button.o \
 display_field.o \
 window.o \
 info_dbox.o \
 menu_item.o \
 menu.o \
 application.o

all: $(OBJ)

include $(OBJ:.o=.d)
