// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/window.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/close_window.h"
#include "rtk/events/leaving_window.h"
#include "rtk/events/entering_window.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/key_pressed.h"
#include "rtk/events/message.h"

namespace rtk {
namespace desktop {

window::window(point size):
	_child(0),
	_handle(0),
	_created(false),
	_dbox_opened(false),
	_movable(true),
	_back_icon(true),
	_close_icon(true),
	_title_bar(true),
	_toggle_icon(false),
	_x_scroll_bar(false),
	_y_scroll_bar(false),
	_adjust_icon(false),
	_button(10),
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

window::~window()
{
	if (_child) _child->remove();
	remove();
}

window* window::as_window()
{
	return this;
}

component* window::find(const point& p) const
{
	bool found=_child&&((p-_child->origin())<=_child->bbox());
	return (found)?_child:0;
}

box window::bbox() const
{
	return _bbox;
}

box window::min_bbox() const
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

void window::resize() const
{
	if (_child) _child->resize();
	inherited::resize();
}

void window::reformat(const point& origin,const box& bbox)
{
	// Update origin and bounding box of this component.
	_bbox=bbox;
	inherited::reformat(origin,bbox);

	// Create RISC OS window.
	create();

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
		window* p=parent_work_area();
		int phandle=(p)?p->handle():-1;

		// Get window state (to preserve position in window stack).
		block1.handle=_handle;
		os::Wimp_GetWindowState(block1);

		// Open window.
		block2.handle=_handle;
		block2.bbox=vabox;
		block2.scroll=scroll;
		block2.behind=block1.behind;
		os::Wimp_OpenWindow(block2,phandle,0);

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
		// Reformat child.
		_child->reformat(corigin,extent);
	}
}

void window::reformat(const point& origin,const box& bbox,size_type level)
{
	if (!_dbox_opened)
	{
		// Reformat in usual manner.  (This does cause the window to be opened
		// in the usual manner, but doing so does not appear to cause any
		// significant harm.)
		reformat(origin,bbox);
		// Calculate required top left-hand corner, with respect to screen.
		point p=bbox.xminymax();
		parent_application(p);
		// Open as dialogue box.
		if (!level) os::Wimp_CreateMenu(_handle,p);
		else os::Wimp_CreateSubMenu(_handle,p);
		// Prevent window being unnecessarily reopened.
		_dbox_opened=true;
	}
}

void window::unformat()
{
	if (_child) _child->unformat();
	uncreate();
	_dbox_opened=false;
	inherited::unformat();
}

void window::remove_notify(component& c)
{
	if (_child==&c)
	{
		_child=0;
	}
}

void window::baseline_notify(const point& offset)
{
	_bbox-=offset;
	if (_child)
	{
		_child->origin(_child->origin()-offset);
	}
}

window& window::add(component& c)
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

string window::title() const
{
	return (_title)?string(_title):string();
}

window& window::title(const string& title,size_type capacity)
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

window& window::back_icon(bool value)
{
	_back_icon=value;
	invalidate();
	return *this;
}

window& window::close_icon(bool value)
{
	_close_icon=value;
	invalidate();
	return *this;
}

window& window::title_bar(bool value)
{
	_title_bar=value;
	invalidate();
	return *this;
}

window& window::toggle_icon(bool value)
{
	_toggle_icon=value;
	invalidate();
	return *this;
}

window& window::x_scroll_bar(bool value)
{
	_x_scroll_bar=value;
	invalidate();
	return *this;
}

window& window::y_scroll_bar(bool value)
{
	_y_scroll_bar=value;
	invalidate();
	return *this;
}

window& window::adjust_icon(bool value)
{
	_adjust_icon=value;
	invalidate();
	return *this;
}

window& window::wb_colour(int colour)
{
	_wb_colour=colour;
	force_redraw();
	return *this;
}

window& window::wf_colour(int colour)
{
	_wf_colour=colour;
	force_redraw();
	return *this;
}

window& window::movable(bool value)
{
	_movable=value;
	return *this;
}

void window::redraw(gcontext& context,const box& clip)
{
	// Windows are redrawn when they receive a Redraw_Window_Request
	// event.  They are not redrawn when an ancestor receives an event
	// of that type.  Therefore, window::redraw() does not allow
	// redraw requests to propogate downwards.
	inherited::redraw(context,clip);
}

void window::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
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

			// Set child origin if there is a child.
			if (_child)
			{
				point corigin=bbox.xminymax()-block.scroll;
				_child->origin(corigin);
			}

			// Reformat this component.
			reformat(origin,bbox);
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
				component* target=this;
				point offset;
				parent_application(offset);
				point position=
					point(wimpblock.word[0],wimpblock.word[1])-offset;
				while (component* child=target->find(position))
				{
					target=child;
					position-=child->origin();
				}
				events::mouse_click ev(*target,wimpblock);
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
				events::key_pressed ev(*this,wimpblock);
				ev.post();
			}
		}
		break;
	case 17:
	case 18:
		{
			events::message ev(*this,wimpblock);
			ev.post();
		}
		break;
	default:
		{
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void window::create()
{
	if (!_created)
	{
		static os::window_create block;
		block.bbox=box();
		block.scroll=point();
		block.behind=-1;
		block.wflags=window_flags();
		block.tf_colour=7;
		block.tb_colour=2;
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

		if (application* app=parent_application())
			app->register_window(*this);
		_created=true;
	}
}

void window::uncreate()
{
	if (_created)
	{
		if (application* app=parent_application())
			app->deregister_window(*this);
		os::window_close block1;
		block1.handle=_handle;
		os::Wimp_CloseWindow(block1);
		os::window_delete block2;
		block2.handle=_handle;
		os::Wimp_DeleteWindow(block2);
		_handle=0;
		_created=false;
	}
}

int window::window_flags() const
{
	return 0x8000c002|
		(_back_icon<<24)|(_close_icon<<25)|(_title_bar<<26)|(_toggle_icon<<27)|
		(_y_scroll_bar<<28)|(_adjust_icon<<29)|(_x_scroll_bar<<30);
}

int window::work_area_flags() const
{
	return _button<<12;
}

void window::register_icon(icon& ic)
{
	_ihandles[ic.handle()]=&ic;
}

void window::deregister_icon(icon& ic)
{
	_ihandles.erase(ic.handle());
}

icon* window::find_icon(int handle) const
{
	map<int,icon*>::const_iterator f=_ihandles.find(handle);
	return (f!=_ihandles.end())?(*f).second:0;
}

box window::calculate_extent(const box& bbox)
{
	box extent;
	if (_child)
	{
		// Create baseline sets.
		xbaseline_set xbs;
		ybaseline_set ybs;

		// Add minimum bounding box of child to baseline sets.
		box mcbbox(_child->min_bbox());
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
