// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/reopen_menu.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

reopen_menu::reopen_menu(component& target):
	event(target)
{}

reopen_menu::~reopen_menu()
{}

bool reopen_menu::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
