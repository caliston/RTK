// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
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
#include "rtk/events/mouse_click.h"
#include "rtk/events/message.h"
#include "rtk/events/menusdeleted.h"

namespace rtk {
namespace desktop {

menu::menu():
	_mdata(0),
	_opened(false),
	_auto_reopen(true),
	_tf_colour(7),
	_tb_colour(2),
	_wf_colour(7),
	_wb_colour(0),
	_height(44),
	_gap(0),
	_title(0),
	_titlesize(0)
{}

menu::~menu()
{
	for (vector<menu_item*>::iterator i=_items.begin();i!=_items.end();++i)
	{
		if (menu_item* c=*i) c->remove();
	}
	uncreate();
}

box menu::min_bbox() const
{
	if (!size_valid()) resize();
	return _min_bbox;
}

void menu::resize() const
{
	int xsize=16;
	int ysize=0;
	if (_title)
	{
		int width=0;
		os::Wimp_TextOp1(_title,0,&width);
		xsize+=width;
	}
	for (vector<menu_item*>::const_iterator i=_items.begin();
		i!=_items.end();++i)
	{
		if (i!=_items.begin()) ysize+=_gap;
		ysize+=_height;
		if (menu_item* c=*i)
		{
			c->resize();
			xsize=max(xsize,c->min_bbox().xsize());
			if (c->separator()) ysize+=24;
		}
	}
	box mbbox(0,0,xsize,ysize);
	mbbox-=external_origin(mbbox,xbaseline_left,ybaseline_top);
	_min_bbox=mbbox;
	inherited::resize();
}

void menu::reformat(const point& origin,const box& pbbox)
{
	inherited::reformat(origin,pbbox);
}

void menu::reformat(const point& origin,const box& pbbox,size_type level)
{
	inherited::reformat(origin,pbbox);
	if (!_opened)
	{
		create();
		point p=pbbox.xminymax();
		application* app=parent_application(p);
		if (!level) os::Wimp_CreateMenu(&**_mdata,p);
		else os::Wimp_CreateSubMenu(&**_mdata,p);
		if (app) app->register_menu_data(_mdata,level);
		_opened=true;
	}
}

void menu::unformat()
{
	for (vector<menu_item*>::const_iterator i=_items.begin();
		i!=_items.end();++i)
	{
		if (menu_item* c=*i) c->unformat();
	}
	_opened=false;
	inherited::unformat();
}

void menu::remove_notify(component& c)
{
	vector<menu_item*>::iterator f=
		std::find(_items.begin(),_items.end(),&c);
	if (f!=_items.end())
	{
		*f=0;
		invalidate();
	}
}

menu& menu::cells(size_type ycells)
{
	for (size_type i=min(ycells,_items.size());i!=_items.size();++i)
		if (menu_item* mi=_items[i]) mi->remove();
	_items.resize(ycells,0);
	invalidate();
	return *this;
}

menu& menu::add(menu_item& item,size_type y)
{
	if (y==npos) y=_items.size();
	if (y>=_items.size()) cells(y+1);
	if (_items[y]) _items[y]->remove();
	_items[y]=&item;
	link_child(item);
	invalidate();
	return *this;
}

menu& menu::title(const string& value,size_type capacity)
{
	size_type length=value.length();
	if (capacity<length) capacity=length;
	if (capacity>_titlesize)
	{
		delete[] _title;
		_title=0;
		_titlesize=0;
		_title=new char[capacity+1];
		_titlesize=capacity;
	}
	if (_title)
	{
		size_type i=value.copy(_title,_titlesize);
		_title[i]=0;
	}
	invalidate();
	return *this;
}

menu& menu::auto_reopen(bool value)
{
	_auto_reopen=value;
	invalidate();
	return *this;
}

menu& menu::tf_colour(int tf_colour)
{
	_tf_colour=tf_colour;
	invalidate();
	return *this;
}

menu& menu::tb_colour(int tb_colour)
{
	_tb_colour=tb_colour;
	invalidate();
	return *this;
}

menu& menu::wf_colour(int wf_colour)
{
	_wf_colour=wf_colour;
	invalidate();
	return *this;
}

menu& menu::wb_colour(int wb_colour)
{
	_wb_colour=wb_colour;
	invalidate();
	return *this;
}

void menu::create()
{
	// Calculate required size in words.
	unsigned int count=_items.size();
	if (count==0) count=1;
	unsigned int size=7+6*count;

	// If required size!=current size then replace with new menu data.
	if ((!_mdata)||(_mdata->size()!=size))
	{
		if (_mdata)
		{
			_mdata->dec_count();
			_mdata=0;
		}
		_mdata=new menu_data(size);
		_mdata->inc_count();
	}

	(*_mdata)->title.pointer[0]=_title;
	(*_mdata)->title.pointer[1]=0;
	(*_mdata)->title.word[2]=_titlesize;
	(*_mdata)->tf_colour=_tf_colour;
	(*_mdata)->tb_colour=_tb_colour;
	(*_mdata)->wf_colour=_wf_colour;
	(*_mdata)->wb_colour=_wb_colour;

	(*_mdata)->width=min_bbox().xsize();
	(*_mdata)->height=_height;
	(*_mdata)->gap=_gap;
	int index=0;
	vector<menu_item*>::iterator i=_items.begin();
	while (i!=_items.end())
	{
		(*_mdata)->item[index].mflags=(*i)->menu_flags();
		(*_mdata)->item[index].submenu=(*i)->dummy_submenu_handle();
		(*_mdata)->item[index].iflags=(*i)->icon_flags();
		(*_mdata)->item[index].icon=(*i)->icon_data();
		++index;
		++i;
	}
	if (index==0)
	{
		(*_mdata)->item[0].mflags=0;
		(*_mdata)->item[0].submenu=-1;
		(*_mdata)->item[0].iflags=0;
		index++;
	}
	(*_mdata)->item[0].mflags|=0x100;
	(*_mdata)->item[index-1].mflags|=0x80;
}

void menu::uncreate()
{
	// Do we need to ensure that it has been closed?
	_opened=false;
	if (_mdata)
	{
		_mdata->dec_count();
		_mdata=0;
	}
}

int menu::handle() const
{
	return (_mdata)?(int)(&**_mdata):0;
}

void menu::reopen()
{
	_opened=false;
	invalidate();
}

void menu::show(const events::mouse_click& ev)
{
	// Translate pointer location from coordinate space of target
	// to coordinate space of screen.
	component* target=ev.target();
	point p=ev.position();
	application* app=target->parent_application(p);
	point offset=min_bbox().xminymax();

	if ((dynamic_cast<icon*>(target))&&
		(dynamic_cast<application*>(target->parent())))
	{
		// If target is an icon on the icon bar then open menu so that
		// its bottom left-hand corner is 64 OS units to the left of
		// the pointer and 96 OS units above the bottom of the screen
		// (as specified in the style guide).
		if (app)
			app->add(*this,point(p.x()-64,96+min_bbox().ysize())-offset);
	}
	else
	{
		// In any other case, open menu so that its top left-hand
		// corner is 64 OS units to the left of the pointer and the
		// top of the first menu item is aligned with the pointer
		// (as specified in the style guide).
		if (app)
			app->add(*this,point(p.x()-64,p.y())-offset);
	}
}

void menu::deliver_wimp_block(int wimpcode,os::wimp_block &wimpblock,
	unsigned int level)
{
	switch (wimpcode)
	{
	case 9:
		{
			unsigned int index=wimpblock.word[level];
			if ((index<_items.size())&&_items[index])
				_items[index]->deliver_wimp_block(wimpcode,wimpblock,level);
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

void menu::deliver_message(int wimpcode,os::wimp_block &wimpblock,
	unsigned int level)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_MenuWarning:
		{
			unsigned int index=wimpblock.word[8+level];
			if (index<_items.size())
			{
				_items[index]->deliver_wimp_block(wimpcode,
					wimpblock,level);
				// The menu tree must be updated as submenu handles
				// become known, so that it can be re-opened correctly
				// if the user clicks with adjust.
				(*_mdata)->item[index].submenu=
					_items[index]->submenu_handle();
			}
		}
		break;
	case swi::Message_MenusDeleted:
		{
			events::menusdeleted ev(*this);
			ev.post();
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

menu::menu_data::menu_data(unsigned int size):
	_size(size),
	_data(new int[_size])
{}

menu::menu_data::~menu_data()
{
	delete[] _data;
}

}; /* namespace desktop */
}; /* namespace rtk */
