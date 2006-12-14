// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/menu_item.h"
#include "rtk/events/menu_selection.h"

namespace rtk {
namespace events {

using rtk::desktop::component;
using rtk::desktop::menu_item;

menu_selection::menu_selection(menu_item& target,int buttons):
	event(target),
	_buttons(buttons)
{}

menu_selection::menu_selection(menu_item& target):
	event(target)
{
	os::pointer_info_get block;
	os::Wimp_GetPointerInfo(block);
	_buttons=block.buttons;
}

menu_selection::~menu_selection()
{}

bool menu_selection::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
