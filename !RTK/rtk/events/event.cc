// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/event.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

event::event(component& target):
	_target(&target)
{}

event::~event()
{}

void event::target(component& target)
{
	_target=&target;
}

bool event::post()
{
	bool handled=false;
	component* dest=_target;
	while (dest)
	{
		// Request parent before delivering event, in case component
		// does not exist after event has been handled.
		component* parent=dest->redirected_parent();
		handled|=deliver(*dest);
		dest=parent;
	}
	return handled;
}

}; /* namespace events */
}; /* namespace rtk */
