// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/swi/os.h"
#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/window.h"
#include "rtk/desktop/menu.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/null_reason.h"
#include "rtk/events/message.h"

namespace rtk {
namespace desktop {

application::application(const string& name):
	_name(name),
	_dbox(0),
	_dbox_level(0),
	_quit(false)
{
	static int messages[]={0};
	os::Wimp_Initialise(380,_name.c_str(),messages,0,&_handle);
}

application::~application()
{
	// Removing windows implicitly causes them to deregister.
	while (_windows.size())
		_windows.back()->remove();
	// Similarly for icons.
	while (_icons.size())
		_icons.back()->remove();
	// Similarly for menus.
	remove_menus(0);
	remove_menu_data(0);
}

application* application::as_application()
{
	return this;
}

box application::min_bbox() const
{
	// The bounding box of the application object is defined to be the
	// bounding box of the desktop.
	int xeigfactor=0;
	int yeigfactor=0;
	int xwindlimit=0;
	int ywindlimit=0;
	os::OS_ReadModeVariable(swi::XEigFactor,&xeigfactor);
	os::OS_ReadModeVariable(swi::YEigFactor,&yeigfactor);
	os::OS_ReadModeVariable(swi::XWindLimit,&xwindlimit);
	os::OS_ReadModeVariable(swi::YWindLimit,&ywindlimit);
	return box(0,0,(xwindlimit+1)<<xeigfactor,(ywindlimit+1)<<yeigfactor);
}

void application::resize() const
{
	// Resize child windows.
	for (vector<window*>::const_iterator i=_windows.begin();
		i!=_windows.end();++i)
	{
		if (!(*i)->size_valid()) (*i)->resize();
	}
	// Resize child icons.
	for (vector<icon*>::const_iterator i=_icons.begin();
		i!=_icons.end();++i)
	{
		if (!(*i)->size_valid()) (*i)->resize();
	}
	// Resize menus.
	for (vector<menu*>::const_iterator i=_menus.begin();i!=_menus.end();++i)
	{
		if (!(*i)->size_valid()) (*i)->resize();
	}
	// Resize dialogue box.
	if (_dbox)
	{
		if (!_dbox->layout_valid()) _dbox->resize();
	}
	inherited::resize();
}

void application::reformat(const point& origin,const box& bbox)
{
	inherited::reformat(origin,bbox);
	// Reformat child windows.
	for (vector<window*>::iterator i=_windows.begin();i!=_windows.end();++i)
	{
		// The application object does not exercise any influence over
		// window locations, so they are reformatted to their last known
		// position and size.
		if (!(*i)->layout_valid())
			(*i)->reformat((*i)->origin(),(*i)->bbox());
	}
	// Reformat child icons.
	for (vector<icon*>::iterator i=_icons.begin();i!=_icons.end();++i)
	{
		// Reformat so that bottom left-hand corner of each icon is at (0,0).
		if (!(*i)->layout_valid())
		{
			box mcbbox=(*i)->min_bbox();
			(*i)->reformat(-mcbbox.xminymin(),mcbbox);
		}
	}
	// Reformat menus.
	for (vector<menu*>::iterator i=_menus.begin();i!=_menus.end();++i)
	{
		if (!(*i)->layout_valid())
		{
			size_type level=i-_menus.begin();
			point cpos=(*i)->origin();
			box mcbbox=(*i)->min_bbox();
			(*i)->reformat(cpos,mcbbox,level);
		}
	}
	// Reformat dialogue box.
	if (_dbox)
	{
		if (!_dbox->layout_valid())
		{
			point cpos=_dbox->origin();
			box mcbbox=_dbox->min_bbox();
			_dbox->reformat(cpos,mcbbox,_dbox_level);
		}
	}
}

void application::remove_notify(component& c)
{
	// Dynamic casts are used here to avoid unnecessary searches.
	if (window* w=dynamic_cast<window*>(&c))
	{
		// Remove if child window.
		vector<window*>::iterator f=
			std::find(_windows.begin(),_windows.end(),w);
		if (f!=_windows.end())
		{
			_windows.erase(f,f+1);
		}
		// Remove if dialogue box.
		if (_dbox==w)
		{
			remove_menus(_dbox_level+1);
			_dbox=0;
			_dbox_level=0;
		}
	}
	else if (icon* ic=dynamic_cast<icon*>(&c))
	{
		// Remove if child icon.
		vector<icon*>::iterator f=
			std::find(_icons.begin(),_icons.end(),ic);
		if (f!=_icons.end())
		{
			_icons.erase(f,f+1);
		}
	}
	else if (menu* m=dynamic_cast<menu*>(&c))
	{
		// Remove if menu.
		vector<menu*>::iterator f=
			std::find(_menus.begin(),_menus.end(),m);
		if (f!=_menus.end())
		{
			int level=f-_menus.begin();
			remove_menus(level+1);
			_menus[level]=0;
		}
	}
}

application& application::add(window& w,const point& p)
{
	w.origin(p);
	_windows.push_back(&w);
	link_child(w);
	invalidate();
	return *this;
}

application& application::add(icon& ic)
{
	_icons.push_back(&ic);
	link_child(ic);
	invalidate();
	return *this;
}

application& application::add(window& w,const point& p,size_type level)
{
	// Remove existing menus at or below this level.
	remove_menus(level);
	remove_menu_data(level);

	// Remove window from its current parent, if any.
	w.remove();

	// Set window origin.
	w.origin(p);

	// Make window a child of this component.
	_dbox=&w;
	_dbox_level=level;
	link_child(w);

	// Invalidate this component (to ensure that the dialogue box
	// is reformatted) and return.
	invalidate();
	return *this;
}

application& application::add(menu& m,const point& p,size_type level)
{
	// Ensure that child pointer array includes this level.
	if (level>=_menus.size()) _menus.resize(level+1,0);

	// Remove existing menus at or below this level.
	for (size_type i=level;i!=_menus.size();++i)
		if (menu* cm=_menus[i]) cm->remove();

	// Remove existing dialogue box if at or below this level.
	if (_dbox&&(_dbox_level>=level))
		_dbox->remove();

	// At this point it is an error if the menu to be added has a
	// parent (because that would make the menu structure cyclic).
	if (m.parent()) throw string("Menu structure must be acyclic");

	// Set menu origin.
	m.origin(p);

	// Make menu a child of this component.
	_menus[level]=&m;
	link_child(m);

	// Invalidate this component (to ensure that the menu is
	// reformatted) and return.
	invalidate();
	return *this;
}

void application::run()
{
	while (!_quit)
	{
		try
		{
			// Ensure layout valid before polling Wimp.
			if (!size_valid()) resize();
			if (!layout_valid()) reformat(point(0,0),box(0,0,0,0));
			// Poll Wimp.
			rtk::graphics::vdu_gcontext::current(0);
			static os::wimp_block wimpblock;
			int wimpcode;
			os::Wimp_Poll(0,wimpblock,0,&wimpcode);
			// Act on returned event block.
			deliver_wimp_block(wimpcode,wimpblock);
			// Send up to one message from queue.
			if (_message_queue.size())
			{
				message msg=_message_queue.front();
				_message_queue.pop();
				os::Wimp_SendMessage(msg.wimpcode,*msg.wimpblock,
					msg.handle,0,0);
				delete msg.wimpblock;
			}
		}
		// Catch exceptions, display using Wimp_ReportError
		// (extracting text from exception where suitable method known).
		catch (std::exception &ex)
		{
			os::Wimp_ReportError(0,ex.what(),name().c_str(),0,0);
		}
		catch (const char* ex)
		{
			os::Wimp_ReportError(0,ex,name().c_str(),0,0);
		}
		catch (const string& ex)
		{
			os::Wimp_ReportError(0,ex.c_str(),name().c_str(),0,0);
		}
		catch (...)
		{
			os::Wimp_ReportError(0,"Unknown exception",name().c_str(),0,0);
		}
	}
}

void application::terminate()
{
	_quit=true;
}

void application::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpcode)
	{
	case 0:
		{
			events::null_reason ev(*this);
			ev.post();
		}
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 8:
	case 10:
	case 11:
	case 12:
		if (window* w=find_window(wimpblock.word[0]))
			w->deliver_wimp_block(wimpcode,wimpblock);
		break;
	case 6:
		if (wimpblock.word[3]==-2)
		{
			if (icon* ic=find_icon(wimpblock.word[4]))
				ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			if (window* w=find_window(wimpblock.word[3]))
				w->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case 9:
		if (_menus.size()&&_menus[0])
			_menus[0]->deliver_wimp_block(wimpcode,wimpblock,0);
		break;
	case 17:
	case 18:
		deliver_message(wimpcode,wimpblock);
		break;
	default:
		{
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void application::deliver_message(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_Quit:
		{
			events::quit ev(*this);
			ev.post();
		}
		break;
	case swi::Message_MenuWarning:
		if (_menus.size()&&_menus[0])
			_menus[0]->deliver_wimp_block(wimpcode,wimpblock,0);
		break;
	case swi::Message_MenusDeleted:
		if (window* w=find_window(wimpblock.word[5]))
		{
			w->deliver_wimp_block(wimpcode,wimpblock);
			w->remove();
		}
		else if (menu* m=find_menu(wimpblock.word[5]))
		{
			m->deliver_wimp_block(wimpcode,wimpblock,0);
			m->remove();
			remove_menu_data(0);
		}
		break;
	default:
		{
			events::message ev(*this,wimpblock);
			ev.post();
		}
		break;
	}
}

void application::send_message(int wimpcode,os::wimp_block& wimpblock,
	int handle)
{
	message msg;
	msg.wimpcode=wimpcode;
	msg.wimpblock=&wimpblock;
	msg.handle=handle;
	_message_queue.push(msg);
}

void application::handle_event(events::quit &ev)
{
	terminate();
}

void application::handle_event(events::reopen_menu &ev)
{
	if (_menus.size()&&_menus[0])
	{
		if (_menus[0]->auto_reopen()) _menus[0]->reopen();
	}
}

void application::register_window(window& w)
{
	_whandles[w.handle()]=&w;
}

void application::register_icon(icon& ic)
{
	_ihandles[ic.handle()]=&ic;
}

void application::register_menu_data(util::refcount* mdata,unsigned int level)
{
	mdata->inc_count();
	if (level>=_mdata.size()) _mdata.resize(level+1,0);
	if (_mdata[level]) _mdata[level]->dec_count();
	_mdata[level]=mdata;
}

void application::deregister_window(window& w)
{
	_whandles.erase(w.handle());
}

void application::deregister_icon(icon& ic)
{
	_ihandles.erase(ic.handle());
}

window* application::find_window(int handle) const
{
	map<int,window*>::const_iterator f=_whandles.find(handle);
	return (f!=_whandles.end())?(*f).second:0;
}

icon* application::find_icon(int handle) const
{
	map<int,icon*>::const_iterator f=_ihandles.find(handle);
	return (f!=_ihandles.end())?(*f).second:0;
}

menu* application::find_menu(int handle) const
{
	return (_menus.size()&&_menus[0]&&(_menus[0]->handle()==handle))?
		_menus[0]:0;
}

void application::remove_menus(size_type level)
{
	// Remove existing menus at or below this level.
	// (This is done in reverse order to avoid recursion.)
	for (size_type i=_menus.size();i>level;--i)
		if (menu* cm=_menus[i-1]) cm->remove();

	// Remove existing dialogue box if at or below this level.
	if (_dbox&&(_dbox_level>=level))
		_dbox->remove();
}

void application::remove_menu_data(size_type level)
{
	// Remove menu data at or below this level.
	for (size_type i=_mdata.size();i>level;--i)
	{
		if (_mdata[i-1]) _mdata[i-1]->dec_count();
		_mdata[i-1]=0;
	}
}

}; /* namespace desktop */
}; /* namespace rtk */