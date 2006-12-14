// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/save_to_app.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

save_to_app::save_to_app(component& target,const string& leafname):
	event(target),
	_leafname(leafname)
{
	os::pointer_info_get block;
	os::Wimp_GetPointerInfo(block);
	_whandle=block.whandle;
	_ihandle=block.ihandle;
	_position=block.p;
};

save_to_app::save_to_app(component& target,int whandle,int ihandle,
	const point& position,const string& leafname):
	event(target),
	_whandle(whandle),
	_ihandle(ihandle),
	_position(position),
	_leafname(leafname)
{}

save_to_app::~save_to_app()
{}

bool save_to_app::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
