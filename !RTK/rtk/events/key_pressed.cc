// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/key_pressed.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

key_pressed::key_pressed(component& target,int code):
	event(target),
	_code(code)
{}

key_pressed::key_pressed(component& target,os::wimp_block& wimpblock):
	event(target),
	_code(wimpblock.word[6])
{}

key_pressed::~key_pressed()
{}

bool key_pressed::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
