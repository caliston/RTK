// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/events/leaving_window.h"

namespace rtk {
namespace events {

using rtk::desktop::component;
using rtk::desktop::basic_window;

leaving_window::leaving_window(basic_window& target):
	event(target)
{}

leaving_window::~leaving_window()
{}

bool leaving_window::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
