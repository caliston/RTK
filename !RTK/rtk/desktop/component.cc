// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <typeinfo>

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/window.h"

// Coordinates within a component are specified with respect to its own
// origin or that of its parent.  This allows a component to be moved
// without modifying all of its children, but makes finding the absolute
// position of an arbitrary component a relatively expensive operation.
// The cost is by keeping track of the required offset when performing
// recursive operations (such as redraw) that use absolute coordinates.

namespace rtk {
namespace desktop {

component::component(const point& origin):
	_parent(0),
	_origin(origin),
	_xbaseline(xbaseline_centre),
	_ybaseline(ybaseline_centre),
	_size_valid(false),
	_layout_valid(false),
	_no_redirect(false),
	_forced_redraw(true),
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

window* component::parent_work_area() const
{
	component* pc=_parent;
	while (pc)
	{
		if (window* pw=pc->as_window()) return pw;
		pc=pc->_parent;
	}
	return 0;
}

window* component::parent_work_area(point& p) const
{
	const component* c=this;
	component* pc=c->_parent;
	while (pc)
	{
		if (window* pw=pc->as_window()) return pw;
		p+=c->_origin;
		c=pc;
		pc=c->_parent;
	}
	p+=c->_origin;
	return 0;
}

application* component::parent_application() const
{
	component* pc=_parent;
	while (pc)
	{
		if (application* pa=pc->as_application()) return pa;
		pc=pc->_parent;
	}
	return 0;
}

application* component::parent_application(point& p) const
{
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

window* component::as_window()
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

component& component::origin(const point& origin)
{
	// Don't do anything unless a change is required.
	if (origin!=_origin)
	{
		// Component will not be able to detect that it has moved when
		// reformat() is called, so it must force a redraw now - but not
		// if it is a window or a work area.
		force_redraw(true);
		// Move component to specified location.
		_origin=origin;
		// Invalidate component, so that the parent (if there is one) can
		// either accept the new location or override it.
		invalidate();
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

box component::bbox() const
{
	return min_bbox();
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

void component::reformat(const point& origin,const box& bbox)
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

void component::force_redraw(bool supress_window)
{
	component* p=parent();
	bool is_window=as_window();
	bool is_work_area=p&&p->as_window();
	supress_window&=is_window||is_work_area;

	if (!_forced_redraw&&!supress_window)
	{
		point offset;
		window* w=as_window();
		if (!w) w=parent_work_area(offset);
		box b=bbox()+offset;
		int h=(w)?w->handle():-1;
		if (h) os::Wimp_ForceRedraw(h,b);
		_forced_redraw=true;
	}
}

void component::set_caret_position(point p,int height,int index)
{
	window* w=as_window();
	if (!w) w=parent_work_area(p);
	if (w)
	{
		os::Wimp_SetCaretPosition(w->handle(),-1,p,height,index);
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

component* component::_redirected_parent() const
{
	component* rp=_parent;
	if (const redirection* r=dynamic_cast<const redirection*>(this))
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

component::redirection::~redirection()
{}

void component::redirection::redirect(component* redirect)
{
	_redirect=redirect;
}

}; /* namespace desktop */
}; /* namespace rtk */
