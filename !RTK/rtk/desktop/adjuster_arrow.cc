// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/adjuster_arrow.h"

namespace rtk {
namespace desktop {

adjuster_arrow::adjuster_arrow(arrow_type type,int delta):
	_delta(delta)
{
	switch (type)
	{
	case arrow_left:
		sprite_name("left");
		if (!_delta) _delta=-1;
		break;
	case arrow_right:
		sprite_name("right");
		if (!_delta) _delta=+1;
		break;
	case arrow_down:
		sprite_name("down");
		if (!_delta) _delta=-1;
		break;
	case arrow_up:
		sprite_name("up");
		if (!_delta) _delta=+1;
		break;
	}
	hcentre(true).vcentre(true);
	fcolour(7).bcolour(1);
	button(2);
	yfit(false);
}

void adjuster_arrow::handle_event(events::mouse_click& ev)
{
	events::arrow_click ev_arrow(ev,_delta);
	ev_arrow.post();
}

}; /* namespace desktop */
}; /* namespace rtk */
