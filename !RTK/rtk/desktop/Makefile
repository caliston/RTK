# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

include ../../Rules.make

OBJ = component.o \
 sizeable_component.o \
 absolute_layout.o \
 row_layout.o \
 column_layout.o \
 grid_layout.o \
 virtual_row_layout.o \
 virtual_column_layout.o \
 virtual_grid_layout.o \
 card_layout.o \
 label.o \
 icon.o \
 action_button.o \
 default_button.o \
 option_button.o \
 radio_button.o \
 display_field.o \
 writable_field.o \
 progress_bar.o \
 window.o \
 prog_info_dbox.o \
 info_dbox.o \
 save_dbox.o \
 menu_item.o \
 menu.o \
 application.o

all: $(OBJ)

include $(OBJ:.o=.d)
