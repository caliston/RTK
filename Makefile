# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2005 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

PREFIX = RTK-0.7.1

all: bin doc dist

bin:
	make -C !RTK bin

doc:
	make -C !RTK doc

dist: dist-src dist-bin dist-doc

dist-src:
	rm -f $(PREFIX).src.zip
	rm -f $(PREFIX).!RTK
	rm -rf $(PREFIX)
	mkdir $(PREFIX)
	ln -s !RTK $(subst .,/,$(PREFIX)).!RTK
	cp Makefile $(PREFIX)
	zip -r $(PREFIX).src.zip $(PREFIX) -i@!RTK/include.src
	rm -f $(PREFIX).!RTK
	rm -rf $(PREFIX)

dist-bin:
	rm -f $(PREFIX).zip
	zip -r $(PREFIX).zip !RTK -i@!RTK/include.bin

dist-doc:
	rm -f $(PREFIX).doc.zip
	zip -r $(PREFIX).doc.zip !RTK -i@!RTK/include.doc
