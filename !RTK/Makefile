# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2005 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

AR = libfile

DOXYFILES = \
 Doxyfile \
 footer.html \
 mainpage.h \
 namespace.h

.PHONY: bin
bin: rtk.a

.PHONY: doc
doc: html/index.html

.PHONY: all
all: bin doc

.PHONY: clean
clean:
	rm -f rtk.a
	make -C rtk clean

rtk.a: rtk/timestamp
	mkdir -p a
	$(AR) -c a.rtk $(wildcard rtk/util/*.o)
	$(AR) -i a.rtk $(wildcard rtk/graphics/*.o)
	$(AR) -i a.rtk $(wildcard rtk/os/*.o)
	$(AR) -i a.rtk $(wildcard rtk/desktop/*.o)
	$(AR) -i a.rtk $(wildcard rtk/events/*.o)
	$(AR) -i a.rtk $(wildcard rtk/transfer/*.o)

rtk/timestamp: always
	make -C rtk

html/index.html: rtk/timestamp $(DOXYFILES)
	doxygen

.PHONY: always
