# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

AR = libfile

bin: always
	make -C rtk
	mkdir -p a
	$(AR) -c a.rtk rtk.util.o.*
	$(AR) -i a.rtk rtk.graphics.o.*
	$(AR) -i a.rtk rtk.os.o.*
	$(AR) -i a.rtk rtk.desktop.o.*
	$(AR) -i a.rtk rtk.events.o.*
	$(AR) -i a.rtk rtk.transfer.o.*

doc: always
	doxygen

all: bin doc

always:
