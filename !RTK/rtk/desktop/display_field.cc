// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/display_field.h"

namespace rtk {
namespace desktop {

display_field::display_field()
{
	validation("R2");
	border(true);
	hcentre(true).vcentre(true);
	fcolour(7).bcolour(1);
	yfit(false);
}

}; /* namespace desktop */
}; /* namespace rtk */
