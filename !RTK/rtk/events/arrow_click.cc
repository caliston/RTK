// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/arrow_click.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

arrow_click::arrow_click(const mouse_click& click,int steps):
	mouse_click(click),
	_steps(steps)
{}

arrow_click::~arrow_click()
{}

bool arrow_click::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
