// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/message.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

message::message(component& target,os::wimp_block& wimpblock):
	event(target),
	_wimpblock(&wimpblock)
{}

message::~message()
{}

int message::thandle() const
{
	return _wimpblock->word[1];
}

int message::msgcode() const
{
	return _wimpblock->word[4];
}

bool message::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
