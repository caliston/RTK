# This file is part of the RISC OS Toolkit (RTK).
# Copyright © 2003-2005 Graham Shaw.
# Distribution and use are subject to the GNU Lesser General Public License,
# a copy of which may be found in the file !RTK.Copyright.

SUBDIRS = util \
 graphics \
 os \
 desktop \
 events \
 transfer

.PHONY: all
all: timestamp

.PHONY: clean
clean:
	rm -f $(SUBDIRS:=/timestamp)
	rm -f $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.[do]))

timestamp: $(SUBDIRS:=/timestamp)
	touch timestamp

%/timestamp: always
	make -C $* -f ../Makefile.subdir

.PHONY: always
