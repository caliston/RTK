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
}

}; /* namespace desktop */
}; /* namespace rtk */
