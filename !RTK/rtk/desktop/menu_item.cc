// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/window.h"
#include "rtk/desktop/menu_item.h"
#include "rtk/desktop/menu.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/menu_selection.h"
#include "rtk/events/message.h"
#include "rtk/events/menuwarning.h"

namespace rtk {
namespace desktop {

menu_item::menu_item():
	_has_submenu(false),
	_has_dbox(false),
	_separator(false)
{
	bcolour(0);
}

menu_item::~menu_item()
{}

menu* menu_item::submenu()
{
	return (_has_submenu)?_submenu:0;
}

menu_item& menu_item::attach_submenu(menu& submenu)
{
	_submenu=&submenu;
	_has_submenu=true;
	_has_dbox=false;
	return *this;
}

menu_item& menu_item::detach_submenu()
{
	if (_has_submenu)
	{
		_has_submenu=false;
		_submenu=0;
	}
	return *this;
}

window* menu_item::dbox()
{
	return (_has_dbox)?_dbox:0;
}

menu_item& menu_item::attach_dbox(window& dbox)
{
	_dbox=&dbox;
	_has_submenu=false;
	_has_dbox=true;
	return *this;
}

menu_item& menu_item::detach_dbox()
{
	if (_has_dbox)
	{
		_has_dbox=false;
		_dbox=0;
	}
	return *this;
}

menu_item& menu_item::separator(bool value)
{
	_separator=value;
	invalidate();
	return *this;
}

void menu_item::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock,
	unsigned int level)
{
	switch (wimpcode)
	{
	case 9:
		{
			// Test whether selection occurred at current level
			// or further down the heirarchy.
			int index=wimpblock.word[level+1];
			if (index==-1)
			{
				// If selection occurred at current level then
				// post menu selection event to this component.
				events::menu_selection ev(*this);
				if (ev.buttons()&1)
				{
					// If selection was made using the adjust button
					// then re-open menu tree.
					events::reopen_menu ev2(*this);
					ev2.post();
				}
				ev.post();
			}
			else
			{
				// If selection occurred further down the heirarchy
				// then deliver block to submenu (assuming one is
				// present, which should be the case).
				if (_has_submenu)
				{
					_submenu->deliver_wimp_block(wimpcode,wimpblock,level+1);
				}
			}
		}
		break;
	case 17:
	case 18:
		deliver_message(wimpcode,wimpblock,level);
		break;
	default:
		{
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void menu_item::deliver_message(int wimpcode,os::wimp_block& wimpblock,
	unsigned int level)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_MenuWarning:
		{
			// Test whether selection occurred at current level
			// or further down the heirarchy.
			int index=wimpblock.word[8+level+1];
			if (index==-1)
			{
				// If selection occurred at current level then
				// automatically open submenu or dialogue box
				// (assuming one or other of these is present,
				// which should be the case).
				// Do not set delivered flag.
				point p(wimpblock.word[6],wimpblock.word[7]);
				if (_has_submenu)
				{
					if (application* app=parent_application())
					{
						point offset=_submenu->min_bbox().xminymax();
						app->add(*_submenu,p-offset,level+1);
						_submenu->redirect(this);
					}
				}
				else if (_has_dbox)
				{
					if (application* app=parent_application())
					{
						point offset=_dbox->min_bbox().xminymax();
						app->add(*_dbox,p-offset,level+1);
					}
				}

				// Post menuwarning event.
				events::menuwarning ev(*this);
				ev.post();
			}
			else
			{
				// If selection occurred further down the heirarchy
				// then deliver block to submenu (assuming one is
				// present, which should be the case).
				if (_has_submenu)
					_submenu->deliver_wimp_block(wimpcode,wimpblock,level+1);
			}
		}
		break;
	default:
		{
			events::message ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

int menu_item::menu_flags() const
{
	int flags=0;
	if (_separator) flags|=2;
	if (_has_submenu||_has_dbox) flags|=8;
	return flags;
}

int menu_item::submenu_handle() const
{
	if (_has_submenu)
	{
		if (_submenu->handle()==0) _submenu->create();
		return _submenu->handle();
	}
	else if (_has_dbox)
	{
		if (_dbox->handle()==0) _dbox->create();
		return _dbox->handle();
	}
	else return 0;
}

int menu_item::dummy_submenu_handle() const
{
	if (_has_submenu)
	{
		if (_submenu->handle()) return _submenu->handle();
		else return (int)this;
	}
	else if (_has_dbox)
	{
		if (_dbox->handle()) return _dbox->handle();
		else return (int)this;
	}
	else return 0;
}

}; /* namespace desktop */
}; /* namespace rtk */
