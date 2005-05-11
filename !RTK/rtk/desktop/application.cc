// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/swi/os.h"
#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/os/dragasprite.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/menu.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/null_reason.h"
#include "rtk/events/user_drag_box.h"
#include "rtk/events/message.h"
#include "rtk/events/claim_entity.h"
#include "rtk/events/datarequest.h"
#include "rtk/transfer/load.h"
#include "rtk/transfer/save.h"

namespace rtk {
namespace desktop {

application::application(const string& name):
	_name(name),
	_dbox(0),
	_dbox_level(0),
	_current_drag(0),
	_drag_sprite(0),
	_current_selection(0),
	_current_clipboard(0),
	_current_load(0),
	_current_save(0),
	_wimp_mask(0),
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
	// Similarly for load and save operations.
	if (_current_load) _current_load->remove();
	if (_current_save) _current_save->remove();
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
	for (std::vector<basic_window*>::const_iterator i=_windows.begin();
		i!=_windows.end();++i)
	{
		if (!(*i)->size_valid()) (*i)->resize();
	}
	// Resize child icons.
	for (std::vector<icon*>::const_iterator i=_icons.begin();
		i!=_icons.end();++i)
	{
		if (!(*i)->size_valid()) (*i)->resize();
	}
	// Resize menus.
	for (std::vector<menu*>::const_iterator i=_menus.begin();
		i!=_menus.end();++i)
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

void application::reformat(const point& origin,const box& pbbox)
{
	inherited::reformat(origin,pbbox);
	// Reformat child windows.
	for (std::vector<basic_window*>::iterator i=_windows.begin();
		i!=_windows.end();++i)
	{
		// If a window has an adjust icon or a scroll bar, the application
		// object does not exercise any influence over its position or
		// size.  (The values passed to reformat() are those returned by
		// origin() and bbox().)

		// If a window does not have an adjust icon or a scroll bar (for
		// a given direction), its size is forced to match the minimum
		// bounding box.
		if (!(*i)->layout_valid())
		{
			box cbbox=(*i)->bbox();
			box mcbbox=(*i)->min_bbox();
			if (!(*i)->x_scroll_bar()&&!(*i)->adjust_icon())
			{
				cbbox.xmin(mcbbox.xmin());
				cbbox.xmax(mcbbox.xmax());
			}
			if (!(*i)->y_scroll_bar()&&!(*i)->adjust_icon())
			{
				cbbox.ymin(mcbbox.ymin());
				cbbox.ymax(mcbbox.ymax());
			}
			(*i)->reformat((*i)->origin(),cbbox);
		}
	}
	// Reformat child icons.
	for (std::vector<icon*>::iterator i=_icons.begin();i!=_icons.end();++i)
	{
		// Reformat so that bottom left-hand corner of each icon is at (0,0).
		if (!(*i)->layout_valid())
		{
			box mcbbox=(*i)->min_bbox();
			point cpos(-mcbbox.xminymin());
			if (mcbbox.ysize()>68) cpos-=point(0,(mcbbox.ysize()-68)/2);
			(*i)->reformat(cpos,mcbbox);
		}
	}
	// Reformat menus.
	for (std::vector<menu*>::iterator i=_menus.begin();i!=_menus.end();++i)
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
	if (basic_window* w=dynamic_cast<basic_window*>(&c))
	{
		// Remove if child window.
		std::vector<basic_window*>::iterator f=
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
		std::vector<icon*>::iterator f=
			std::find(_icons.begin(),_icons.end(),ic);
		if (f!=_icons.end())
		{
			_icons.erase(f,f+1);
		}
	}
	else if (menu* m=dynamic_cast<menu*>(&c))
	{
		// Remove if menu.
		std::vector<menu*>::iterator f=
			std::find(_menus.begin(),_menus.end(),m);
		if (f!=_menus.end())
		{
			int level=f-_menus.begin();
			remove_menus(level+1);
			_menus[level]=0;
		}
	}
	else if (&c==_current_load)
	{
		_current_load=0;
	}
	else if (&c==_current_save)
	{
		_current_save=0;
	}
}

application& application::add(basic_window& w,const point& p)
{
	w.remove();
	w.origin(p);
	_windows.push_back(&w);
	link_child(w);
	invalidate();
	return *this;
}

application& application::add(icon& ic)
{
	ic.remove();
	_icons.push_back(&ic);
	link_child(ic);
	invalidate();
	return *this;
}

application& application::add(basic_window& w,const point& p,size_type level)
{
	// This statement is needed to prevent the menu tree being closed
	// when an open dialogue box is re-opened.
	if (_dbox==&w) _dbox=0;

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

application& application::add(transfer::load& loadop)
{
	loadop.remove();
	_current_load=&loadop;
	link_child(loadop);
	invalidate();
	return *this;
}

application& application::add(transfer::save& saveop)
{
	saveop.remove();
	_current_save=&saveop;
	link_child(saveop);
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
			os::Wimp_Poll(_wimp_mask,wimpblock,0,&wimpcode);
			// Act on returned event block.
			deliver_wimp_block(wimpcode,wimpblock);
			// Send up to one message from queue.
			if (_message_queue.size())
			{
				message msg=_message_queue.front();
				_message_queue.pop();
				os::Wimp_SendMessage(msg.wimpcode,*msg.wimpblock,
					msg.whandle,msg.ihandle,0);
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
			bool found=false;
			for (std::vector<component*>::iterator i=_null.begin();
				i!=_null.end();++i)
			{
				events::null_reason ev(**i);
				found|=ev.post();
			}
			if (!found) _wimp_mask|=1;
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
		if (basic_window* w=find_window(wimpblock.word[0]))
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
			if (basic_window* w=find_window(wimpblock.word[3]))
				w->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case 7:
		if (_current_drag)
		{
			if (_drag_sprite) os::DragASprite_Stop();
			component* target=_current_drag;
			deregister_drag(*target);
			events::user_drag_box ev(*target,wimpblock);
			ev.post();
		}
		break;
	case 9:
		if (_menus.size()&&_menus[0])
			_menus[0]->deliver_wimp_block(wimpcode,wimpblock,wimpblock.word,0);
		break;
	case 17:
	case 18:
		deliver_message(wimpcode,wimpblock);
		break;
	case 19:
		deliver_message_ack(wimpcode,wimpblock);
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
	case swi::Message_DataSave:
	case swi::Message_DataLoad:
		if (wimpblock.word[5]==-2)
		{
			if (icon* ic=find_icon(wimpblock.word[6]))
				ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			if (basic_window* w=find_window(wimpblock.word[5]))
				w->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case swi::Message_RAMTransmit:
		{
			if (_current_load)
				_current_load->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case swi::Message_DataSaveAck:
	case swi::Message_DataLoadAck:
	case swi::Message_RAMFetch:
		{
			if (_current_save)
				_current_save->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case swi::Message_DataOpen:
		{
			events::dataopen ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_ClaimEntity:
		if (wimpblock.word[1]!=_handle)
		{
			if (wimpblock.word[5]&3)
			{
				if (_current_selection)
				{
					events::claim_entity ev(*_current_selection,
						wimpcode,wimpblock);
					ev.post();
				}
				_current_selection=0;
			}
			if (wimpblock.word[5]&4)
			{
				if (_current_clipboard)
				{
					events::claim_entity ev(*_current_clipboard,
						wimpcode,wimpblock);
					ev.post();
				}
				_current_clipboard=0;
			}
		}
		break;
	case swi::Message_DataRequest:
		{
			if (_current_clipboard)
			{
				events::datarequest ev(*_current_clipboard,
					wimpcode,wimpblock);
				ev.post();
			}
		}
		break;
	case swi::Message_HelpRequest:
		if (wimpblock.word[8]==-2)
		{
			if (icon* ic=find_icon(wimpblock.word[9]))
				ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else if (basic_window* w=find_window(wimpblock.word[8]))
		{
			w->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			int whandle=wimpblock.word[8];
			int ihandle=wimpblock.word[9];
			int tree[64];
			os::Wimp_GetMenuState(whandle,ihandle,tree);
			if (_menus.size()&&_menus[0])
				_menus[0]->deliver_wimp_block(wimpcode,wimpblock,tree,0);
		}
		break;
	case swi::Message_MenuWarning:
		if (_menus.size()&&_menus[0])
			_menus[0]->deliver_wimp_block(wimpcode,wimpblock,
				wimpblock.word+8,0);
		break;
	case swi::Message_MenusDeleted:
		if (basic_window* w=find_window(wimpblock.word[5]))
		{
			w->deliver_wimp_block(wimpcode,wimpblock);
			w->remove();
		}
		else if (menu* m=find_menu(wimpblock.word[5]))
		{
			m->deliver_wimp_block(wimpcode,wimpblock);
			m->remove();
			remove_menu_data(0);
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

void application::deliver_message_ack(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_DataOpen:
		{
			events::datasave ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataSave:
	case swi::Message_DataLoad:
	case swi::Message_RAMTransmit:
		{
			if (_current_save)
				_current_save->deliver_wimp_block(wimpcode,wimpblock);
		}
		break;
	case swi::Message_RAMFetch:
		{
			if (_current_load)
				_current_load->deliver_wimp_block(wimpcode,wimpblock);
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

void application::send_message(int wimpcode,os::wimp_block& wimpblock,
	int whandle,int ihandle)
{
	message msg;
	msg.wimpcode=wimpcode;
	msg.wimpblock=&wimpblock;
	msg.whandle=whandle;
	msg.ihandle=ihandle;
	_message_queue.push(msg);
}

transfer::load* application::auto_load(unsigned int filetype)
{
	return 0;
}

component* application::current_selection() const
{
	return _current_selection;
}

component* application::current_clipboard() const
{
	return _current_clipboard;
}

void application::handle_event(events::quit &ev)
{
	terminate();
}

void application::handle_event(events::datasave &ev)
{
	if (std::find(_icons.begin(),_icons.end(),
		dynamic_cast<icon*>(ev.target()))!=_icons.end())
	{
		if (transfer::load* loadop=auto_load(ev.filetype()))
		{
			add(*loadop);
			ev.target(*loadop);
			ev.post();
		}
	}
}

void application::handle_event(events::dataload &ev)
{
	if (ev.target()==this)
	{
		if (transfer::load* loadop=auto_load(ev.filetype()))
		{
			add(*loadop);
			ev.target(*loadop);
			ev.post();
		}
	}
}

void application::handle_event(events::dataopen &ev)
{
	if (ev.target()==this)
	{
		if (transfer::load* loadop=auto_load(ev.filetype()))
		{
			add(*loadop);
			ev.target(*loadop);
			ev.post();
		}
	}
}

void application::handle_event(events::reopen_menu &ev)
{
	if (_menus.size()&&_menus[0])
	{
		if (_menus[0]->auto_reopen()) 
		{
			for (std::vector<menu*>::iterator i=_menus.begin();
				i!=_menus.end();++i)
			{
				if (!(*i)->layout_valid())
				{
					size_type level=i-_menus.begin();
					point cpos=(*i)->origin();
					box mcbbox=(*i)->min_bbox();
					(*i)->reformat(cpos,mcbbox,level);
				}
			}
			_menus[0]->reopen();
		}
	}
}

void application::register_window(basic_window& w)
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

void application::register_null(component& c)
{
	std::vector<component*>::iterator f=
		std::find(_null.begin(),_null.end(),&c);
	if (f==_null.end()) _null.push_back(&c);
	_wimp_mask&=~1;
}

void application::register_drag(component& c,bool sprite)
{
	_current_drag=&c;
	_drag_sprite=sprite;
}

void application::register_selection(component& c)
{
	_current_selection=&c;
}

void application::register_clipboard(component& c)
{
	_current_clipboard=&c;
}

void application::deregister_window(basic_window& w)
{
	if ((&w==_dbox)&&(_dbox_level==0))
	{
		os::Wimp_CreateMenu(-1,point());
	}
	_whandles.erase(w.handle());
}

void application::deregister_icon(icon& ic)
{
	_ihandles.erase(ic.handle());
}

void application::deregister_null(component& c)
{
	std::vector<component*>::iterator f=
		std::find(_null.begin(),_null.end(),&c);
	if (f!=_null.end()) _null.erase(f);
}

void application::deregister_drag(component& c)
{
	if (_current_drag==&c) _current_drag=0;
	_drag_sprite=false;
}

void application::deregister_selection(component& c)
{
	if (_current_selection==&c) _current_selection=0;
}

void application::deregister_clipboard(component& c)
{
	if (_current_clipboard==&c) _current_clipboard=0;
}

basic_window* application::find_window(int handle) const
{
	std::map<int,basic_window*>::const_iterator f=_whandles.find(handle);
	return (f!=_whandles.end())?(*f).second:0;
}

icon* application::find_icon(int handle) const
{
	std::map<int,icon*>::const_iterator f=_ihandles.find(handle);
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
