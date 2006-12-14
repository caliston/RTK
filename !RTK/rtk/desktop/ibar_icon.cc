// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/menu.h"
#include "rtk/desktop/ibar_icon.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/application.h"

namespace rtk {
namespace desktop {

ibar_icon::ibar_icon():
	_select_window(0),
	_adjust_window(0),
	_menu(0)
{
	button(3);
}

ibar_icon::ibar_icon(rtk::desktop::menu& m):
	_select_window(0),
	_adjust_window(0),
	_menu(&m)
{
	button(3);
}

void ibar_icon::handle_event(rtk::events::mouse_click& ev)
{
	if (ev.buttons()==2)
	{
		if (_menu) _menu->show(ev);
	}
	else if (ev.buttons()&4)
	{
		if (rtk::desktop::application* app=parent_application())
		{
			if (_select_window)
			{
				app->add(*_select_window,_select_window->origin());
			}
		}
	}
	else if (ev.buttons()&1)
	{
		if (rtk::desktop::application* app=parent_application())
		{
			if (_adjust_window)
			{
				app->add(*_adjust_window,_adjust_window->origin());
			}
		}
	}
}

ibar_icon& ibar_icon::attach_select_window(basic_window& w)
{
	_select_window=&w;
	return *this;
}

ibar_icon& ibar_icon::attach_adjust_window(basic_window& w)
{
	_adjust_window=&w;
	return *this;
}

ibar_icon& ibar_icon::attach_menu(rtk::desktop::menu& m)
{
	_menu=&m;
	return *this;
}

ibar_icon& ibar_icon::detach_select_window()
{
	_select_window=0;
	return *this;
}

ibar_icon& ibar_icon::detach_adjust_window()
{
	_adjust_window=0;
	return *this;
}

ibar_icon& ibar_icon::detach_menu()
{
	_menu=0;
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
