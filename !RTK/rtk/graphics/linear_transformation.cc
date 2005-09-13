// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <iostream>

#include "rtk/graphics/linear_transformation.h"

namespace rtk {
namespace graphics {

inline int linear_transformation::mul(int x,int y)
{
	long long z=static_cast<long long>(x)*static_cast<long long>(y);
	return z/scale;
}

point linear_transformation::operator()(const point& p) const
{
	return point(mul(_xx,p.x())+mul(_yx,p.y()),mul(_xy,p.x())+mul(_yy,p.y()));
}

linear_transformation linear_transformation::operator()(
	const linear_transformation& t) const
{
	return linear_transformation(
		mul(_xx,t.xx())+mul(_yx,t.xy()),mul(_xy,t.xx())+mul(_yy,t.xy()),
		mul(_xx,t.yx())+mul(_yx,t.yy()),mul(_xy,t.yx())+mul(_yy,t.yy()));
}

std::ostream& operator<<(std::ostream& out,const linear_transformation& t)
{
	out << '(' << t.xx() << ',' << t.xy() << ','
		<< t.yx() << ',' << t.yy() << ')';
	return out;
}

}; /* namespace graphics */
}; /* namespace rtk */
