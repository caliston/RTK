// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/close_window.h"
#include "rtk/events/leaving_window.h"
#include "rtk/events/entering_window.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/key_pressed.h"
#include "rtk/events/message.h"
#include "rtk/events/help_request.h"

namespace rtk {
namespace desktop {

basic_window::basic_window(point size):
	_child(0),
	_handle(0),
	_created(false),
	_registered(false),
	_opened(false),
	_dbox(false),
	_movable(false),
	_back_icon(false),
	_close_icon(false),
	_title_bar(false),
	_toggle_icon(false),
	_x_scroll_bar(false),
	_y_scroll_bar(false),
	_adjust_icon(false),
	_border(false),
	_button(10),
	_tf_colour(7),
	_tb_colour(2),
	_wf_colour(7),
	_wb_colour(1),
	_title(0),
	_titlesize(0)
{
	// Initialise baselines first, then the bounding box
	// (so that the bounding box does not change)
	xbaseline(xbaseline_left);
	ybaseline(ybaseline_top);
	_bbox=box(0,-size.y(),size.x(),0);
}

basic_window::~basic_window()
{
	if (_child) _child->remove();
	remove();
	uncreate();
}

basic_window* basic_window::as_window()
{
	return this;
}

component* basic_window::find(const point& p) const
{
	bool found=_child&&((p-_child->origin())<=_child->bbox());
	return (found)?_child:0;
}

box basic_window::bbox() const
{
	return _bbox;
}

box basic_window::min_bbox() const
{
	int xsize=0;
	int ysize=0;
	if (_child)
	{
		box mcbbox=_child->min_bbox();
		xsize=mcbbox.xsize();
		ysize=mcbbox.ysize();
	}
	box mbbox(0,-ysize,xsize,0);
	mbbox-=external_origin(mbbox,xbaseline_left,ybaseline_top);
	return mbbox;
}

void basic_window::resize() const
{
	if (_child) _child->resize();
	inherited::resize();
}

void basic_window::reformat(const point& origin,const box& pbbox)
{
	_dbox=false;
	_reformat(origin,pbbox,behind());
}

void basic_window::reformat(const point& origin,const box& pbbox,size_type level)
{
	if (!_opened||!_dbox)
	{
		// Window is a dialogue box.
		_dbox=true;

		// Reformat in usual manner.  (This does cause the window to be opened
		// in the usual manner, but doing so does not appear to cause any
		// significant harm.)
		_reformat(origin,pbbox,behind());

		// Calculate required top left-hand corner, with respect to screen.
		point p=pbbox.xminymax();
		parent_application(p);

		// Open as dialogue box.
		if (!level) os::Wimp_CreateMenu(_handle,p);
		else os::Wimp_CreateSubMenu(_handle,p);
		_opened=true;
	}
}

void basic_window::unformat()
{
	if (_child) _child->unformat();
	if (_opened)
	{
		os::window_close block;
		block.handle=_handle;
		os::Wimp_CloseWindow(block);
		_opened=false;
		_dbox=false;
	}
	if (_registered)
	{
		if (application* app=parent_application())
			app->deregister_window(*this);
		_registered=false;
	}
	inherited::unformat();
}

void basic_window::remove_notify(component& c)
{
	if (_child==&c)
	{
		_child=0;
	}
}

void basic_window::baseline_notify(const point& offset)
{
	_bbox-=offset;
	if (_child)
	{
		_child->origin(_child->origin()-offset);
	}
}

basic_window& basic_window::add(component& c)
{
	if (_child) _child->remove();
	_child=&c;
	link_child(*_child);
	invalidate();

	// Calculate extent (taking into account size of visible area
	// and minimum bounding box of child).  This must be done after
	// the child has been added (otherwise calculate_extent() will
	// not work properly).
	box extent=calculate_extent(_bbox);

	// Set child origin so that the top left-hand corner of the
	// work area is at the top left-hand corner of the visible area.
	c.origin(_bbox.xminymax()-extent.xminymax());

	return *this;
}

string basic_window::title() const
{
	return (_title)?string(_title):string();
}

basic_window& basic_window::title(const string& title,size_type capacity)
{
	// Calculate required title string buffer capacity.
	size_type length=title.length();
	if (capacity<length) capacity=length;
	// If current buffer is too small (or non-existant) then allocate
	// new buffer.  Window will be re-created if necessary.
	if (capacity>_titlesize)
	{
		invalidate();
		delete[] _title;
		_titlesize=0;
		_title=new char[capacity+1];
		_titlesize=capacity;
	}
	// If buffer has been created then copy title string into buffer
	// and append terminating zero.
	if (_title)
	{
		size_type i=title.copy(_title,_titlesize);
		_title[i]=0;
	}
	// Force redraw and return.
	force_redraw();
	return *this;
}

basic_window& basic_window::back_icon(bool value)
{
	_back_icon=value;
	invalidate();
	return *this;
}

basic_window& basic_window::close_icon(bool value)
{
	_close_icon=value;
	invalidate();
	return *this;
}

basic_window& basic_window::title_bar(bool value)
{
	_title_bar=value;
	invalidate();
	return *this;
}

basic_window& basic_window::toggle_icon(bool value)
{
	_toggle_icon=value;
	invalidate();
	return *this;
}

basic_window& basic_window::x_scroll_bar(bool value)
{
	_x_scroll_bar=value;
	invalidate();
	return *this;
}

basic_window& basic_window::y_scroll_bar(bool value)
{
	_y_scroll_bar=value;
	invalidate();
	return *this;
}

basic_window& basic_window::adjust_icon(bool value)
{
	_adjust_icon=value;
	invalidate();
	return *this;
}

basic_window& basic_window::border(bool value)
{
	_border=value;
	return *this;
}

basic_window& basic_window::button(int value)
{
	_button=value;
	return *this;
}

basic_window& basic_window::tb_colour(int colour)
{
	_tb_colour=colour;
	return *this;
}

basic_window& basic_window::tf_colour(int colour)
{
	_tf_colour=colour;
	return *this;
}

basic_window& basic_window::wb_colour(int colour)
{
	_wb_colour=colour;
	return *this;
}

basic_window& basic_window::wf_colour(int colour)
{
	_wf_colour=colour;
	return *this;
}

basic_window& basic_window::movable(bool value)
{
	_movable=value;
	return *this;
}

void basic_window::redraw(gcontext& context,const box& clip)
{
	// Windows are redrawn when they receive a Redraw_Window_Request
	// event.  They are not redrawn when an ancestor receives an event
	// of that type.  Therefore, basic_window::redraw() does not allow
	// redraw requests to propogate downwards.
	inherited::redraw(context,clip);
}

void basic_window::handle_event(events::close_window& ev)
{
	remove();
}

void basic_window::handle_event(events::auto_scroll& ev)
{
	// Translate bounding box coordinates from target to this window.
	point offset;
	parent_application(offset);
	offset=-offset;
	ev.target()->parent_application(offset);
	box lbbox=ev.bbox()+offset;

	// Calculate how far the work area has to move to make the
	// specified bounding box visible.
	point diff;
	if (lbbox.xmin()<bbox().xmin())
	{
		diff.x(lbbox.xmin()-bbox().xmin());
	}
	if (lbbox.xmax()>bbox().xmax())
	{
		diff.x(lbbox.xmax()-bbox().xmax());
	}
	if (lbbox.ymin()<bbox().ymin())
	{
		diff.y(lbbox.ymin()-bbox().ymin());
	}
	if (lbbox.ymax()>bbox().ymax())
	{
		diff.y(lbbox.ymax()-bbox().ymax());
	}

	// Move the work area.
	if (_child&&(diff!=point()))
	{
		_child->origin(_child->origin()-diff,true);
	}
}

void basic_window::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpcode)
	{
	case 1:
		{
			os::window_redraw& block=(os::window_redraw&)wimpblock;
			int more;
			os::Wimp_RedrawWindow(block,&more);
			while (more)
			{
				point origin=block.bbox.xminymax()-block.scroll;
				box clip=block.clip-origin;
				rtk::graphics::vdu_gcontext context(origin);
				if (_child) _child->redraw(context,clip);
				os::Wimp_GetRectangle(block,&more);
			}
		}
		break;
	case 2:
		if (_movable)
		{
			os::window_open& block=(os::window_open&)wimpblock;

			// Calculate origin of parent with respect to
			// origin of screen.
			point parent_origin(-origin());
			parent_application(parent_origin);

			// Calculate bounding box with respect to top left-hand corner.
			box bbox(0,-block.bbox.ysize(),block.bbox.xsize(),0);

			// Translate to external origin.
			bbox-=external_origin(bbox,xbaseline_left,ybaseline_top);

			// Calculate origin required to match bbox to block.bbox.
			point origin=block.bbox.xminymax()-bbox.xminymax()-parent_origin;

			// Set child origin if there is a child
			// (but do not force redraw).
			if (_child)
			{
				point corigin=bbox.xminymax()-block.scroll;
				_child->origin(corigin,true);
			}

			// Reformat this component.
			_reformat(origin,bbox,block.behind);
		}
		break;
	case 3:
		{
			events::close_window ev(*this);
			ev.post();
		}
		break;
	case 4:
		{
			events::leaving_window ev(*this);
			ev.post();
		}
		break;
	case 5:
		{
			events::entering_window ev(*this);
			ev.post();
		}
		break;
	case 6:
		{
			if (icon* ic=find_icon(wimpblock.word[4]))
			{
				ic->deliver_wimp_block(wimpcode,wimpblock);
			}
			else
			{
				point pos(wimpblock.word[0],wimpblock.word[1]);
				events::mouse_click ev(*find_target(pos),wimpblock);
				ev.post();
			}
		}
		break;
	case 8:
		{
			if (icon* ic=find_icon(wimpblock.word[1]))
			{
				ic->deliver_wimp_block(wimpcode,wimpblock);
			}
			else
			{
				component* target=this;
				point position=
					point(wimpblock.word[2],wimpblock.word[3]);
				if (_child) position+=_child->origin();
				while (component* child=target->find(position))
				{
					target=child;
					position-=child->origin();
				}
				events::key_pressed ev(*target,wimpblock);
				ev.post();
			}
		}
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

void basic_window::deliver_message(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_DataSave:
		if (icon* ic=find_icon(wimpblock.word[6]))
		{
			ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			point pos(wimpblock.word[7],wimpblock.word[8]);
			events::datasave ev(*find_target(pos),wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataLoad:
		if (icon* ic=find_icon(wimpblock.word[6]))
		{
			ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			point pos(wimpblock.word[7],wimpblock.word[8]);
			events::dataload ev(*find_target(pos),wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_HelpRequest:
		if (icon* ic=find_icon(wimpblock.word[9]))
		{
			ic->deliver_wimp_block(wimpcode,wimpblock);
		}
		else
		{
			point pos(wimpblock.word[5],wimpblock.word[6]);
			events::help_request ev(*find_target(pos),wimpblock);
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

void basic_window::create()
{
	if (!_created)
	{
		static os::window_create block;
		block.bbox=box();
		block.scroll=point();
		block.behind=-1;
		block.wflags=window_flags();
		block.tf_colour=(_border)?_tf_colour:0xff;
		block.tb_colour=_tb_colour;
		block.wf_colour=_wf_colour;
		block.wb_colour=_wb_colour;
		block.so_colour=3;
		block.si_colour=1;
		block.th_colour=12;
		block.reserved=0;
		block.workarea=_extent;
		block.tflags=(_title)?0x00000109:0x00000000;
		block.waflags=work_area_flags();
		block.spritearea=1;
		block.minxsize=0;
		block.minysize=0;
		block.title.pointer[0]=_title;
		block.title.pointer[1]=0;
		block.title.word[2]=_titlesize;
		block.numicons=0;
		os::Wimp_CreateWindow(block,&_handle);
		_created=true;
	}
}

void basic_window::uncreate()
{
	if (_created)
	{
		if (_opened)
		{
			os::window_close block;
			block.handle=_handle;
			os::Wimp_CloseWindow(block);
			_opened=false;
			_dbox=false;
		}
		if (_registered)
		{
			if (application* app=parent_application())
				app->deregister_window(*this);
			_registered=false;
		}
		os::window_delete block;
		block.handle=_handle;
		os::Wimp_DeleteWindow(block);
		_handle=0;
		_created=false;
	}
}

int basic_window::window_flags() const
{
	return 0x8000c002|
		(_back_icon<<24)|(_close_icon<<25)|(_title_bar<<26)|(_toggle_icon<<27)|
		(_y_scroll_bar<<28)|(_adjust_icon<<29)|(_x_scroll_bar<<30);
}

int basic_window::work_area_flags() const
{
	return _button<<12;
}

int basic_window::behind() const
{
	int behind=-1;
	if (_handle)
	{
		static os::window_state_get block;
		block.handle=_handle;
		os::Wimp_GetWindowState(block);
		behind=block.behind;
	}
	return behind;
}

void basic_window::register_icon(icon& ic)
{
	_ihandles[ic.handle()]=&ic;
}

void basic_window::deregister_icon(icon& ic)
{
	_ihandles.erase(ic.handle());
}

icon* basic_window::find_icon(int handle) const
{
	map<int,icon*>::const_iterator f=_ihandles.find(handle);
	return (f!=_ihandles.end())?(*f).second:0;
}

component* basic_window::find_target(const point& pos)
{
	component* target=this;
	point offset;
	parent_application(offset);
	point lpos=pos-offset;
	while (component* child=target->find(lpos))
	{
		target=child;
		lpos-=child->origin();
	}
	return target;
}

void basic_window::_reformat(const point& origin,const box& pbbox,int behind)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component.
	_bbox=bbox;
	inherited::reformat(origin,bbox);

	// Create RISC OS window.
	create();

	// Register with parent application.
	if (!_registered)
	{
		if (application* app=parent_application())
		{
			app->register_window(*this);
			_registered=true;
		}
	}

	// Calculate extent (taking into account size of visible area
	// and minimum bounding box of child).  Update RISC OS window.
	box extent=calculate_extent(_bbox);
	if (_extent!=extent)
	{
		os::Wimp_SetExtent(_handle,extent);
		_extent=extent;
	}

	// Calculate origin of parent with respect to origin of screen.
	point parent_origin(-this->origin());
	parent_application(parent_origin);

	// The Wimp can override the requested size of a window.  When this
	// happens, the child will be placed incorrectly.  To correct this,
	// the bounding box and extent are recalculated after the window has
	// been opered.  If the extent has changed then the process of
	// opening the window is repeated.  (For safety, only one repeat is
	// allowed.  This should be sufficient, and if it isn't then there
	// is a significant risk of an infinite loop.)
	static os::window_state_get block1;
	static os::window_open block2;
	int attempts=2;
	while (attempts--)
	{
		// Calculate visible area with respect to origin of screen.
		box vabox=bbox+origin+parent_origin;

		// Calculate required scroll offsets.
		// Note that this is done after the window has been resized
		// (therefore the scroll bars can move when the window size changes
		// even if the child origin does not).
		point scroll=(_child)?_bbox.xminymax()-_child->origin():point();

		// Obtain parent window handle (-1 if top-level).
		basic_window* p=parent_work_area();
		int phandle=(p)?p->handle():-1;

		// Open window.
		block2.handle=_handle;
		block2.bbox=vabox;
		block2.scroll=scroll;
		block2.behind=behind;
		os::Wimp_OpenWindow(block2,phandle,0);
		_opened=true;

		// Get window state (which may be different from what was requested
		// when the window was opened).
		block1.handle=_handle;
		os::Wimp_GetWindowState(block1);

		// Update bounding box.
		_bbox=block1.bbox-origin-parent_origin;

		// Recalculate extent.  If unchanged then terminate loop.
		extent=calculate_extent(_bbox);
		if (_extent!=extent)
		{
			os::Wimp_SetExtent(_handle,extent);
			_extent=extent;
		}
		else attempts=0;
	}

	// Reformat child, if there is one.
	if (_child)
	{
		// Calculate child origin.
		point corigin=_bbox.xminymax()-block1.scroll;

		// Set child origin, without forcing redraw.  This makes it less
		// likely that _child->reformat() will cause an unnecessary redraw.
		_child->origin(corigin,true);

		// Reformat child.
		_child->reformat(corigin,extent);
	}
}

box basic_window::calculate_extent(const box& bbox)
{
	box extent;
	if (_child)
	{
		// Create baseline sets.
		xbaseline_set xbs;
		ybaseline_set ybs;

		// Add minimum bounding box of child to baseline sets.
		box mcbbox(_child->min_wrap_bbox(bbox));
		xbs.add(mcbbox,_child->xbaseline());
		ybs.add(mcbbox,_child->ybaseline());

		// Calculate offsets from child origin to child perimeter.
		extent.xmin(
			xbs.offset(_child->xbaseline(),xbaseline_left,bbox.xsize()));
		extent.ymin(
			ybs.offset(_child->ybaseline(),ybaseline_bottom,bbox.ysize()));
		extent.xmax(
			xbs.offset(_child->xbaseline(),xbaseline_right,bbox.xsize()));
		extent.ymax(
			ybs.offset(_child->ybaseline(),ybaseline_top,bbox.ysize()));
	}
	return extent;
}

}; /* namespace desktop */
}; /* namespace rtk */
