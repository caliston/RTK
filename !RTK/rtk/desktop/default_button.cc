// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/default_button.h"

namespace rtk {
namespace desktop {

default_button::default_button()
{
	validation("R6,3");
	border(true);
	hcentre(true).vcentre(true);
	fcolour(7).bcolour(1);
	button(3);
}

}; /* namespace desktop */
}; /* namespace rtk */
