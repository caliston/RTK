// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/absolute_layout.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;

absolute_layout::absolute_layout()
{
	xbaseline(xbaseline_left);
	ybaseline(ybaseline_top);
}

absolute_layout::~absolute_layout()
{
	while (_components.size())
		_components.back()->remove();
	remove();
}

box absolute_layout::auto_bbox() const
{
	box abbox;
	std::vector<component*>::const_iterator i=_components.begin();
	if (i!=_components.end())
	{
		component* c=*i++;
		abbox=(c->min_bbox()+c->origin());
	}
	while (i!=_components.end())
	{
		component* c=*i++;
		abbox|=(c->min_bbox()+c->origin());
	}

	// Add margin.
	abbox|=_margin;

	// Ensure that bounding box is compatible with baselines.
	switch (xbaseline())
	{
	case xbaseline_left:
		abbox.xmin(0);
		break;
	case xbaseline_right:
		abbox.xmax(0);
		break;
	case xbaseline_centre:
		{
			int xsize=max(-abbox.xmin(),abbox.xmax());
			abbox.xmin(-xsize);
			abbox.xmax(xsize);
		}
		break;
	default:
		/* no action. */
		break;
	}
	switch (ybaseline())
	{
	case ybaseline_top:
		abbox.ymax(0);
		break;
	case ybaseline_bottom:
		abbox.ymin(0);
		break;
	case ybaseline_centre:
		{
			int ysize=max(-abbox.ymin(),abbox.ymax());
			abbox.ymin(-ysize);
			abbox.ymax(ysize);
		}
		break;
	default:
		/* no action. */
		break;
	}

	return abbox;
}

component* absolute_layout::find(const point& p) const
{
	for (std::vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=*i;
		if (p<=c->bbox()+c->origin()) return c;
	}
	return 0;
}

box absolute_layout::bbox() const
{
	return _bbox;
}

void absolute_layout::resize() const
{
	for (std::vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=*i;
		c->resize();
	}
	inherited::resize();
}

void absolute_layout::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// any children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	// Place children.
	for (std::vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=*i;
		c->reformat(c->origin(),c->min_bbox());
	}
}

void absolute_layout::unformat()
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=*i;
		c->unformat();
	}
}

void absolute_layout::redraw(gcontext& context,const box& clip)
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i)
		{
			point cpos=c->origin();
			context+=cpos;
			c->redraw(context,clip-cpos);
			context-=cpos;
		}
	}
	inherited::redraw(context,clip);
}

void absolute_layout::remove_notify(component& c)
{
	std::vector<component*>::iterator f=
		std::find(_components.begin(),_components.end(),&c);
	if (f!=_components.end())
	{
		_components.erase(f);
		invalidate();
	}
}

absolute_layout& absolute_layout::add(component& c,const point& p)
{
	c.remove();
	c.origin(p);
	_components.push_back(&c);
	link_child(c);
	invalidate();
	return *this;
}

absolute_layout& absolute_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

absolute_layout& absolute_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

}; /* namespace desktop */
}; /* namespace rtk */
