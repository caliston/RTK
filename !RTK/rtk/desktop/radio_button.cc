// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/radio_button.h"

namespace rtk {
namespace desktop {

radio_button::radio_button()
{
	text_and_sprite(true);
	validation("Sradiooff,radioon");
	vcentre(true);
	fcolour(7);
	button(11);
	yfit(false);
}

void radio_button::handle_event(events::mouse_click& ev)
{
	if (ev.buttons() == 1) {
		// Only select if not already selected, to avoid flicker
		if (!selected()) selected(true);
	}
}

} /* namespace desktop */
} /* namespace rtk */
