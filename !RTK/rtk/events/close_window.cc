// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/desktop/window.h"
#include "rtk/events/close_window.h"

namespace rtk {
namespace events {

using rtk::desktop::component;
using rtk::desktop::window;

close_window::close_window(window& target):
	event(target)
{}

close_window::~close_window()
{}

bool close_window::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
