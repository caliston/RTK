// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/writable_field.h"

namespace rtk {
namespace desktop {

writable_field::writable_field()
{
	validation("Ktar;Pptr_write");
	border(true);
	fill(true);
	hcentre(true).vcentre(true);
	fcolour(7).bcolour(0);
	button(14);
	yfit(false);
	xbaseline(xbaseline_text);
	ybaseline(ybaseline_text);
	min_bbox(box(0,0,16,0));
}

}; /* namespace desktop */
}; /* namespace rtk */
