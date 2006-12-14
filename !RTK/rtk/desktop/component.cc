// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <memory>
#include <algorithm>
#include <typeinfo>

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/os/dragasprite.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/application.h"
#include "rtk/events/claim_entity.h"
#include "rtk/events/redirection.h"

// Coordinates within a component are specified with respect to its own
// origin or that of its parent.  This allows a component to be moved
// without modifying all of its children, but makes finding the absolute
// position of an arbitrary component a relatively expensive operation.
// The cost is by keeping track of the required offset when performing
// recursive operations (such as redraw) that use absolute coordinates.

namespace rtk {
namespace desktop {

using std::min;
using std::max;

component::component(const point& origin):
	_parent(0),
	_origin(origin),
	_xbaseline(xbaseline_centre),
	_ybaseline(ybaseline_centre),
	_size_valid(false),
	_layout_valid(false),
	_no_redirect(false),
	_forced_redraw(true),
	_xfit(true),
	_yfit(true),
	_xauto(true),
	_yauto(true)
{}

component::~component()
{
	set_parent(0);
}

void component::set_parent(component* c)
{
	// Act only if required parent differs from current one.
	if (_parent!=c)
	{
		if (_parent)
		{
			// Unformat /must/ be called before the parent link is
			// broken, so that descendant windows and icons are able
			// to de-register themselves.
			unformat();
			_parent->remove_notify(*this);
		}
		_parent=c;
		// If this component is invalid then its new parent must be
		// invalidated to main consistency of the chain.  The safest
		// place to do that is here.  Furthermore, even if this
		// component is valid, chances are very high that the parent
		// will have to be invalidated anyway.   This makes it
		// cheaper on balance to invalidate unconditionally and
		// accept some unnecessary reformatting than to work out
		// exactly when invalidation is necessary.
		if (_parent) _parent->invalidate();
		_forced_redraw=false;
	}
}

basic_window* component::parent_work_area() const
{
	component* pc=_parent;
	while (pc)
	{
		if (basic_window* pw=pc->as_window()) return pw;
		pc=pc->_parent;
	}
	return 0;
}

basic_window* component::parent_work_area(point& p) const
{
	const component* c=this;
	component* pc=c->_parent;
	while (pc)
	{
		if (basic_window* pw=pc->as_window()) return pw;
		p+=c->_origin;
		c=pc;
		pc=c->_parent;
	}
	p+=c->_origin;
	return 0;
}

application* component::parent_application()
{
	if (application* app=as_application()) return app;
	component* pc=_parent;
	while (pc)
	{
		if (application* pa=pc->as_application()) return pa;
		pc=pc->_parent;
	}
	return 0;
}

application* component::parent_application(point& p)
{
	if (application* app=as_application()) return app;
	const component* c=this;
	component* pc=c->_parent;
	while (pc)
	{
		p+=c->_origin;
		if (application* pa=pc->as_application()) return pa;
		c=pc;
		pc=c->_parent;
	}
	p+=c->_origin;
	return 0;
}

basic_window* component::as_window()
{
	return 0;
}

application* component::as_application()
{
	return 0;
}

component* component::find(const point& p) const
{
	return 0;
}

void component::remove()
{
	set_parent(0);
}

component& component::origin(const point& origin,bool suppress_redraw)
{
	// Don't do anything unless a change is required.
	if (origin!=_origin)
	{
		// Component will not be able to detect that it has moved when
		// reformat() is called, so it must force a redraw now - but not
		// if it is a window, and not if redraws have been suppressed.
		if (!suppress_redraw) force_redraw(true);
		// Move component to specified location.
		_origin=origin;
		// Invalidate parent (if there is one) so that it can either
		// accept the new location or override it.
		if (_parent) _parent->invalidate();
	}
	return *this;
}

void component::xbaseline(xbaseline_type xbaseline)
{
	xbaseline_type old_xbaseline=_xbaseline;
	_xbaseline=xbaseline;
	invalidate();
	point offset=external_origin(bbox(),old_xbaseline,_ybaseline);
	baseline_notify(offset);
}

void component::ybaseline(ybaseline_type ybaseline)
{
	ybaseline_type old_ybaseline=_ybaseline;
	_ybaseline=ybaseline;
	invalidate();
	point offset=external_origin(bbox(),_xbaseline,old_ybaseline);
	baseline_notify(offset);
}

void component::xfit(bool xfit)
{
	_xfit=xfit;
	invalidate();
}

void component::yfit(bool yfit)
{
	_yfit=yfit;
	invalidate();
}

box component::bbox() const
{
	return min_bbox();
}

box component::min_wrap_bbox(const box& wbox) const
{
	return min_bbox();
}

component::wrap_type component::wrap_direction() const
{
	return wrap_none;
}

void component::invalidate()
{
	component* p=this;
	while (p&&p->_size_valid)
	{
		p->_size_valid=false;
		p->_layout_valid=false;
		p=p->_parent;
	}
}

void component::resize() const
{
	_size_valid=true;
}

void component::reformat(const point& origin,const box& pbbox)
{
	// Move origin.
	_origin=origin;
	// Ensure that size is valid before layout is made valid.
	if (!_size_valid) resize();
	// Declare layout to be valid.
	_layout_valid=true;
	// Since origin or bounding box of object may have changed, a future
	// force_redraw() is not necessarily redundant.
	_forced_redraw=false;
}

void component::unformat()
{
	invalidate();
}

void component::redraw(gcontext& context,const box& clip)
{
	_forced_redraw=false;
}

void component::force_redraw(bool suppress_window)
{
	suppress_window&=bool(as_window());

	if (!_forced_redraw&&!suppress_window)
	{
		point offset;
		basic_window* w=as_window();
		if (!w) w=parent_work_area(offset);
		if (int h=(w)?w->handle():-1)
		{
			os::Wimp_ForceRedraw(h,bbox()+offset);
		}
		_forced_redraw=true;
	}
}

void component::force_redraw(const box& clip)
{
	if (!_forced_redraw)
	{
		point offset;
		basic_window* w=as_window();
		if (!w) w=parent_work_area(offset);
		if (int h=(w)?w->handle():-1)
		{
			os::Wimp_ForceRedraw(h,clip+offset);
		}
	}
}

void component::force_update()
{
	force_update(bbox());
}

void component::force_update(const box& clip)
{
	if (!_forced_redraw)
	{
		point offset;
		basic_window* w=as_window();
		if (!w) w=parent_work_area(offset);
		if (w) w->force_update(clip+offset);
	}
}

void component::block_copy(const box& src,const point& dst)
{
	point offset;
	basic_window* w=as_window();
	if (!w) w=parent_work_area(offset);
	if (w)
	{
		if (int handle=w->handle())
		{
			os::Wimp_BlockCopy(handle,src+offset,dst+offset);
		}
	}
}

void component::set_caret_position(point p,int height,int index)
{
	basic_window* w=as_window();
	if (!w) w=parent_work_area(p);
	if (w)
	{
		os::Wimp_SetCaretPosition(w->handle(),-1,p,height,index);
	}
}

void component::claim_selection()
{
	std::auto_ptr<os::wimp_block> block(new os::wimp_block);
	block->word[0]=24;
	block->word[3]=0;
	block->word[4]=swi::Message_ClaimEntity;
	block->word[5]=3;

	if (desktop::application* app=parent_application())
	{
		if (component* current_selection=app->current_selection())
		{
			if (current_selection!=this)
			{
				events::claim_entity ev(*current_selection,
					swi::User_Message,*block);
				ev.post();
				app->deregister_selection(*current_selection);
			}
		}
		else
		{
			app->send_message(swi::User_Message,*(block.release()),0,0);
		}

		app->register_selection(*this);
	}
}

void component::claim_clipboard()
{
	std::auto_ptr<os::wimp_block> block(new os::wimp_block);
	block->word[0]=24;
	block->word[3]=0;
	block->word[4]=swi::Message_ClaimEntity;
	block->word[5]=4;

	if (desktop::application* app=parent_application())
	{
		if (component* current_clipboard=app->current_clipboard())
		{
			if (current_clipboard!=this)
			{
				events::claim_entity ev(*current_clipboard,
					swi::User_Message,*block);
				ev.post();
				app->deregister_clipboard(*current_clipboard);
			}
		}
		else
		{
			app->send_message(swi::User_Message,*(block.release()),0,0);
		}

	app->register_clipboard(*this);
	}
}

void component::drag(const box& dbox,const box& bbox,int drag_type)
{
	point offset;
	application* app=parent_application(offset);
	if (app)
	{
		os::drag_box block;
		block.type=drag_type;
		block.dbox=dbox+offset;
		block.pbox=bbox+offset;
		block.r12=0;
		block.draw_func=0;
		block.remove_func=0;
		block.move_func=0;
		os::Wimp_DragBox(block);
		app->register_drag(*this);
	}
}

void component::drag(const box& dbox,int drag_type)
{
	point offset;
	application* app=parent_application(offset);
	if (app)
	{
		os::drag_box block;
		block.type=drag_type;
		block.dbox=dbox+offset;
		block.pbox=app->bbox();
		block.r12=0;
		block.draw_func=0;
		block.remove_func=0;
		block.move_func=0;
		os::Wimp_DragBox(block);
		app->register_drag(*this);
	}
}

void component::drag_box(const box& dbox)
{
	drag(dbox,5);
}

void component::drag_sprite(const box& dbox,os::sprite_area* area,
	const string& name)
{
	point offset;
	application* app=parent_application(offset);
	if (app)
	{
		box adbox=dbox+offset;
		os::DragASprite_Start(0xc5,area,name.c_str(),&adbox,0);
		app->register_drag(*this,true);
	}
}

void component::link_child(component& c)
{
	c.set_parent(this);
}

void component::remove_notify(component& c)
{}

void component::baseline_notify(const point& offset)
{
	_origin+=offset;
}

point component::internal_origin(const box& bbox,xbaseline_type ixbaseline,
	ybaseline_type iybaseline) const
{
	point offset(0,0);
	if (xbaseline()!=ixbaseline)
	{
		switch (ixbaseline)
		{
		case xbaseline_text:
		case xbaseline_left:
			offset.x(bbox.xmin());
			break;
		case xbaseline_right:
			offset.x(bbox.xmax());
			break;
		case xbaseline_centre:
			offset.x((bbox.xmin()+bbox.xmax())/2);
			break;
		}
	}
	if (ybaseline()!=iybaseline)
	{
		switch (iybaseline)
		{
		case ybaseline_text:
		case ybaseline_bottom:
			offset.y(bbox.ymin());
			break;
		case ybaseline_top:
			offset.y(bbox.ymax());
			break;
		case ybaseline_centre:
			offset.y((bbox.ymin()+bbox.ymax())/2);
			break;
		}
	}
	return offset;
}

point component::external_origin(const box& bbox,xbaseline_type ixbaseline,
	ybaseline_type iybaseline) const
{
	point offset(0,0);
	if (xbaseline()!=ixbaseline)
	{
		switch (xbaseline())
		{
		case xbaseline_text:
		case xbaseline_left:
			offset.x(bbox.xmin());
			break;
		case xbaseline_right:
			offset.x(bbox.xmax());
			break;
		case xbaseline_centre:
			offset.x((bbox.xmin()+bbox.xmax())/2);
			break;
		}
	}
	if (component::ybaseline()!=iybaseline)
	{
		switch (ybaseline())
		{
		case ybaseline_text:
		case ybaseline_bottom:
			offset.y(bbox.ymin());
			break;
		case ybaseline_top:
			offset.y(bbox.ymax());
			break;
		case ybaseline_centre:
			offset.y((bbox.ymin()+bbox.ymax())/2);
			break;
		}
	}
	return offset;
}

box component::fit(const box& pbbox) const
{
	box bbox=pbbox;
	if ((!_xfit)||(!_yfit))
	{
		box mbbox=min_bbox();
		if (!_xfit)
		{
			bbox.xmin(mbbox.xmin());
			bbox.xmax(mbbox.xmax());
		}
		if (!_yfit)
		{
			bbox.ymin(mbbox.ymin());
			bbox.ymax(mbbox.ymax());
		}
	}
	return bbox;
}

component* component::_redirected_parent() const
{
	component* rp=_parent;
	if (const events::redirection* r=
		dynamic_cast<const events::redirection*>(this))
	{
		if (r->redirect()) rp=r->redirect();
	}
	else
	{
		_no_redirect=true;
	}
	return rp;
}

component::xbaseline_set::xbaseline_set():
	_xmin(0),
	_xmax(0),
	_xsize(0)
{}

component::xbaseline_set& component::xbaseline_set::add(const box& bbox,
	xbaseline_type xbaseline)
{
	switch (xbaseline)
	{
	case xbaseline_text:
		_xmin=min(_xmin,bbox.xmin());
		_xmax=max(_xmax,bbox.xmax());
		break;
	default:
		_xsize=max(_xsize,bbox.xsize());
		break;
	}
	return *this;
}

int component::xbaseline_set::offset(xbaseline_type src_xbaseline,
	xbaseline_type dst_xbaseline,int xsize) const
{
	int xmin(_xmin);
	int xmax(max(_xmax,_xmin+max(_xsize,xsize)));

	int xoffset=0;
	switch (src_xbaseline)
	{
	case xbaseline_left:
		xoffset-=xmin;
		break;
	case xbaseline_right:
		xoffset-=xmax;
		break;
	case xbaseline_centre:
		xoffset-=(xmin+xmax)/2;
		break;
	default:
		break;
	}
	switch (dst_xbaseline)
	{
	case xbaseline_left:
		xoffset+=xmin;
		break;
	case xbaseline_right:
		xoffset+=xmax;
		break;
	case xbaseline_centre:
		xoffset+=(xmin+xmax)/2;
		break;
	default:
		break;
	}
	return xoffset;
}

int component::xbaseline_set::xsize() const
{
	return max(_xsize,_xmax-_xmin);
}

component::ybaseline_set::ybaseline_set():
	_ymin(0),
	_ymax(0),
	_ysize(0)
{}

component::ybaseline_set& component::ybaseline_set::add(const box& bbox,
	ybaseline_type ybaseline)
{
	switch (ybaseline)
	{
	case ybaseline_text:
		_ymin=min(_ymin,bbox.ymin());
		_ymax=max(_ymax,bbox.ymax());
		break;
	default:
		_ysize=max(_ysize,bbox.ysize());
		break;
	}
	return *this;
}

int component::ybaseline_set::offset(ybaseline_type src_ybaseline,
	ybaseline_type dst_ybaseline,int ysize) const
{
	int ymin(_ymin);
	int ymax(max(_ymax,_ymin+max(_ysize,ysize)));

	int yoffset=0;
	switch (src_ybaseline)
	{
	case ybaseline_bottom:
		yoffset-=ymin;
		break;
	case ybaseline_top:
		yoffset-=ymax;
		break;
	case ybaseline_centre:
		yoffset-=(ymin+ymax)/2;
		break;
	default:
		break;
	}
	switch (dst_ybaseline)
	{
	case ybaseline_bottom:
		yoffset+=ymin;
		break;
	case ybaseline_top:
		yoffset+=ymax;
		break;
	case ybaseline_centre:
		yoffset+=(ymin+ymax)/2;
		break;
	default:
		break;
	}
	return yoffset;
}

int component::ybaseline_set::ysize() const
{
	return max(_ysize,_ymax-_ymin);
}

} /* namespace desktop */
} /* namespace rtk */
