// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <iostream>

#include "rtk/graphics/point.h"

namespace rtk {
namespace graphics {

std::ostream& operator<<(std::ostream& out,const point& p)
{
	out << '(' << p.x() << ',' << p.y() << ')';
	return out;
}

}; /* namespace graphics */
}; /* namespace rtk */
