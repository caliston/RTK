// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/window.h"
#include "rtk/desktop/ibar_menu.h"
#include "rtk/desktop/application.h"

namespace rtk {
namespace desktop {

ibar_menu::ibar_menu(const string& name):
	_prog_info_dbox(0),
	_choices_window(0)
{
	title(name);
	_menu_info.text("Info");
	_menu_choices.text("Choices ...");
	_menu_quit.text("Quit");
	rebuild();
}

void ibar_menu::handle_event(rtk::events::menu_selection& ev)
{
	if (ev.target()==&_menu_choices)
	{
		if (rtk::desktop::application* app=parent_application())
		{
			if (_choices_window)
			{
				app->add(*_choices_window,_choices_window->origin());
			}
		}
	}
	else if (ev.target()==&_menu_quit)
	{
		if (rtk::desktop::application* app=parent_application())
		{
			app->terminate();
		}
	}
}

ibar_menu& ibar_menu::attach_prog_info_dbox(window& dbox)
{
	_prog_info_dbox=&dbox;
	_menu_info.attach_dbox(dbox);
	rebuild();
	return *this;
}

ibar_menu& ibar_menu::attach_choices_window(window& w)
{
	_choices_window=&w;
	rebuild();
	return *this;
}

ibar_menu& ibar_menu::detach_prog_info_dbox()
{
	_prog_info_dbox=0;
	rebuild();
	return *this;
}

ibar_menu& ibar_menu::detach_choices_window()
{
	_choices_window=0;
	rebuild();
	return *this;
}

void ibar_menu::rebuild()
{
	cells(0);
	if (_prog_info_dbox)
	{
		add(_menu_info);
	}
	if (_choices_window)
	{
		add(_menu_choices);
	}
	add(_menu_quit);
}

}; /* namespace desktop */
}; /* namespace rtk */
