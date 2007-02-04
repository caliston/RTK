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
	text_and_sprite(true);
	switch (type)
	{
	case arrow_left:
		validation("R5;sleft,pleft");
		if (!_delta) _delta=-1;
		break;
	case arrow_right:
		validation("R5;sright,pright");
		if (!_delta) _delta=+1;
		break;
	case arrow_down:
		validation("R5;sdown,pdown");
		if (!_delta) _delta=-1;
		break;
	case arrow_up:
		validation("R5;sup,pup");
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
	if (ev.buttons()!=2)
	{
		int delta=(ev.buttons()==1) ? -_delta : _delta;
		events::arrow_click ev_arrow(ev,delta);
		ev_arrow.post();
	}
}

} /* namespace desktop */
} /* namespace rtk */
