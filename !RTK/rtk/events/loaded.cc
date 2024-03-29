// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/loaded.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

loaded::loaded(component& target,transfer::basic_load& loadop):
	event(target),
	_loadop(&loadop)
{};

loaded::~loaded()
{}

bool loaded::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
