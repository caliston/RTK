# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2004 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

SUBDIRS = util \
 graphics \
 os \
 desktop \
 events \
 transfer

.PHONY: all always $(SUBDIRS)

all: timestamp

timestamp: $(SUBDIRS:=/timestamp)
	touch timestamp

%/timestamp: always
	make -C $*
