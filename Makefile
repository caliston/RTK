# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

PREFIX = RTK-0/7/0

all: bin doc dist

bin:
	make -C !RTK bin

doc:
	make -C !RTK doc

dist: dist-src dist-bin dist-doc

dist-src:
	Remove $(PREFIX)/src/zip
	CDir $(PREFIX)
	Wipe $(PREFIX) ~CFR~V
	CDir $(PREFIX)
	ln -s !RTK $(PREFIX).!RTK
	Copy Makefile $(PREFIX).* ~CF~V
	Copy */lst $(PREFIX).* ~CF~V
	zip -r $(PREFIX)/src/zip $(PREFIX) -i@!RTK.include/src
	Wipe $(PREFIX) ~CFR~V

dist-bin:
	Remove $(PREFIX)/zip
	zip -r $(PREFIX)/zip !RTK -i@!RTK.include/bin

dist-doc:
	Remove $(PREFIX)/doc/zip
	zip -r $(PREFIX)/doc/zip !RTK -i@!RTK.include/doc
