// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/radio_button.h"

namespace rtk {
namespace desktop {

radio_button::radio_button()
{
	validation("Sradiooff,radioon");
	vcentre(true);
	fcolour(7);
	button(11);
	yfit(false);
}

}; /* namespace desktop */
}; /* namespace rtk */
