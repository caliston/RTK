# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

AR = libfile

DOXYFILES = \
 Doxyfile \
 footer.html \
 mainpage.h \
 namespace.h

.PHONY: all always

bin: rtk.a

doc: html/index.html

all: bin doc

rtk.a: rtk/timestamp
	mkdir -p a
	$(AR) -c a.rtk rtk.util.o.*
	$(AR) -i a.rtk rtk.graphics.o.*
	$(AR) -i a.rtk rtk.os.o.*
	$(AR) -i a.rtk rtk.desktop.o.*
	$(AR) -i a.rtk rtk.events.o.*
	$(AR) -i a.rtk rtk.transfer.o.*

rtk/timestamp: always
	Run SetPaths
	make -C rtk

html/index.html: rtk/timestamp $(DOXYFILES)
	doxygen
