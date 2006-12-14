// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/auto_scroll.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

auto_scroll::auto_scroll(component& target,const box& bbox):
	event(target),
	_bbox(bbox)
{};

auto_scroll::~auto_scroll()
{}

bool auto_scroll::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
