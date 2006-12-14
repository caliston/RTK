// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/mouse_click.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

mouse_click::mouse_click(component& target,const point& position,
	int buttons,int shift):
	event(target),
	_position(position),
	_buttons(buttons),
	_shift(shift)
{}

mouse_click::mouse_click(component& target,os::wimp_block& wimpblock):
	event(target),
	_position(point(wimpblock.word[0],wimpblock.word[1])),
	_buttons(wimpblock.word[2]),
	_shift(0)
{
	// Calculate origin of target with respect to origin of screen.
	point origin;
	target.parent_application(origin);

	// Translate pointer location from coordinate space of screen
	// to coordinate space of target.
	_position-=origin;
}

mouse_click::~mouse_click()
{}

bool mouse_click::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
