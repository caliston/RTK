// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/discard.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

discard::discard(component& target):
	event(target)
{}

discard::~discard()
{}

bool discard::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
