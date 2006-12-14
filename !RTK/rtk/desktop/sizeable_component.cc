// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

sizeable_component::sizeable_component(const point& origin):
	component(origin)
{}

sizeable_component::~sizeable_component()
{}

box sizeable_component::min_bbox() const
{
	if (!size_valid()) resize();
	return _min_bbox;
}

void sizeable_component::resize() const
{
	if (xauto()||yauto())
	{
		box b=auto_bbox();
		if (xauto())
		{
			_min_bbox.xmin(b.xmin());
			_min_bbox.xmax(b.xmax());
		}
		if (yauto())
		{
			_min_bbox.ymin(b.ymin());
			_min_bbox.ymax(b.ymax());
		}
	}
	inherited::resize();
}

sizeable_component& sizeable_component::min_bbox(const box& min_bbox)
{
	_min_bbox=min_bbox;
	set_xauto(!_min_bbox.xsize());
	set_yauto(!_min_bbox.ysize());
	return *this;
}

sizeable_component& sizeable_component::min_size(const point& min_size)
{
	_min_bbox=min_bbox();
	if (min_size.x())
	{
		xbaseline_set xbs;
		xbs.add(_min_bbox,xbaseline());
		_min_bbox.xmin(xbs.offset(xbaseline(),xbaseline_left,min_size.x()));
		_min_bbox.xmax(xbs.offset(xbaseline(),xbaseline_right,min_size.x()));
	}
	if (min_size.y())
	{
		ybaseline_set ybs;
		ybs.add(_min_bbox,ybaseline());
		_min_bbox.ymin(ybs.offset(ybaseline(),ybaseline_bottom,min_size.y()));
		_min_bbox.ymax(ybs.offset(ybaseline(),ybaseline_top,min_size.y()));
	}
	set_xauto(!min_size.x());
	set_yauto(!min_size.y());
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
