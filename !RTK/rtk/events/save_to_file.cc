// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/component.h"
#include "rtk/events/save_to_file.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

save_to_file::save_to_file(component& target,const string& pathname,
	bool selection):
	event(target),
	_pathname(pathname),
	_selection(selection)
{}

save_to_file::~save_to_file()
{}

bool save_to_file::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
