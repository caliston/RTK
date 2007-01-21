// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/min_size.h"

namespace rtk {
namespace desktop {

min_size::min_size():
	_content(0)
{
}

min_size::~min_size()
{
	remove();
}

min_size& min_size::add(component& c)
{
	if (_content) _content->remove();
	_content=&c;
	link_child(c);
	invalidate();
	return *this;
}

min_size& min_size::minimum_size(const point& size)
{
	_min_size = size;
	invalidate();
	return *this;
}

box min_size::auto_bbox() const
{
	box abbox;
	// Must be big enough to hold the child
	if (_content) abbox=_content->min_bbox();

	// Ensure the component is at least as big as the minimum size specified
	if (_min_size.x())
	{
		xbaseline_set xbs;
		xbs.add(abbox,xbaseline());
		abbox.xmin(xbs.offset(xbaseline(),xbaseline_left,_min_size.x()));
		abbox.xmax(xbs.offset(xbaseline(),xbaseline_right,_min_size.x()));
	}
	if (_min_size.y())
	{
		ybaseline_set ybs;
		ybs.add(abbox,ybaseline());
		abbox.ymin(ybs.offset(ybaseline(),ybaseline_bottom,_min_size.y()));
		abbox.ymax(ybs.offset(ybaseline(),ybaseline_top,_min_size.y()));
	}

	return abbox;
}

box min_size::bbox() const
{
	box bbox;
	if (_content) bbox=_content->bbox();
	return bbox;
}

void min_size::resize() const
{
	if (_content) _content->resize();
	inherited::resize();
}

void min_size::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// the child.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	if (_content) _content->reformat(point(0,0),bbox);
}

void min_size::unformat()
{
	if (_content) _content->unformat();
	inherited::unformat();
}

void min_size::redraw(gcontext& context,const box& clip)
{
	if (_content) _content->redraw(context,clip);
	inherited::redraw(context,clip);
}

} /* namespace desktop */
} /* namespace rtk */
