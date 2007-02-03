// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/wrappable_component.h"

namespace rtk {
namespace desktop {

wrappable_component::wrappable_component(const point& origin):
	sizeable_component(origin)
{}

wrappable_component::~wrappable_component()
{}

box wrappable_component::min_wrap_bbox(const box& wbox) const
{
	if (!size_valid()) resize();
	switch (wrap_direction())
	{
	case wrap_horizontal:
		if ((_min_wrap_bbox.xmin()!=wbox.xmin())||
			(_min_wrap_bbox.xmax()!=wbox.xmax()))
		{
			_min_wrap_bbox=auto_wrap_bbox(wbox);
		}
		break;
	case wrap_vertical:
		if ((_min_wrap_bbox.ymin()!=wbox.ymin())||
			(_min_wrap_bbox.ymax()!=wbox.ymax()))
		{
			_min_wrap_bbox=auto_wrap_bbox(wbox);
		}
		break;
	case wrap_none:
		break;
	};
	return _min_wrap_bbox;
}

void wrappable_component::resize() const
{
	inherited::resize();
	_min_wrap_bbox=min_bbox();
}

} /* namespace desktop */
} /* namespace rtk */
