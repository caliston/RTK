# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

CXX = gcc
AS = objasm
FIXDEPS = fix-deps

CPPFLAGS = -Irtk:
CXXFLAGS = -mthrowback -munixlib -mpoke-function-name -Wall -W -Wno-unused -O2
ASFLAGS = -throwback

.SUFFIXES: .o .s .cc .d .dd

.s.o:
	$(AS) $(ASFLAGS) $<

.s.d:
	$(FIXDEPS) $* < /dev/null > $@

.cc.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

.cc.dd:
	$(CXX) -MM $(CPPFLAGS) $< > $@

.dd.d:
	$(FIXDEPS) $* < $< > $@

.DELETE_ON_ERROR:
