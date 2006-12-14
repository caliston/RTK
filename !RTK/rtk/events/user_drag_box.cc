// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/user_drag_box.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

user_drag_box::user_drag_box(component& target,const graphics::box& dbox):
	event(target),
	_dbox(dbox)
{}

user_drag_box::user_drag_box(component& target,os::wimp_block& wimpblock):
	event(target),
	_dbox(wimpblock.word[0],wimpblock.word[1],wimpblock.word[2],
		wimpblock.word[3])
{
	// Calculate origin of target with respect to origin of screen.
	point origin;
	target.parent_application(origin);

	// Translate pointer location from coordinate space of screen
	// to coordinate space of target.
	_dbox-=origin;
}

user_drag_box::~user_drag_box()
{}

bool user_drag_box::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
