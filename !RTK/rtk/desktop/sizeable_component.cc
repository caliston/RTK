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

}; /* namespace desktop */
}; /* namespace rtk */
