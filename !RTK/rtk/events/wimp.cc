// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/wimp.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

wimp::wimp(component& target,int wimpcode,os::wimp_block& wimpblock):
	event(target),
	_wimpcode(wimpcode),
	_wimpblock(&wimpblock)
{}

wimp::~wimp()
{}

bool wimp::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
