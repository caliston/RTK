// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/graphics/box.h"

namespace rtk {
namespace graphics {

box& box::operator+=(const point& p)
{
	_xmin+=p.x();
	_ymin+=p.y();
	_xmax+=p.x();
	_ymax+=p.y();
	return *this;
}

box& box::operator-=(const point& p)
{
	_xmin-=p.x();
	_ymin-=p.y();
	_xmax-=p.x();
	_ymax-=p.y();
	return *this;
}

box& box::operator+=(const box& b)
{
	_xmin+=b._xmin;
	_ymin+=b._ymin;
	_xmax+=b._xmax;
	_ymax+=b._ymax;
	return *this;
}

box& box::operator-=(const box& b)
{
	_xmin-=b._xmin;
	_ymin-=b._ymin;
	_xmax-=b._xmax;
	_ymax-=b._ymax;
	return *this;
}

box& box::operator|=(const box& b)
{
	if (_xmin>b._xmin) _xmin=b._xmin;
	if (_ymin>b._ymin) _ymin=b._ymin;
	if (_xmax<b._xmax) _xmax=b._xmax;
	if (_ymax<b._ymax) _ymax=b._ymax;
	return *this;
}

box& box::operator&=(const box& b)
{
	if (_xmin<b._xmin) _xmin=b._xmin;
	if (_ymin<b._ymin) _ymin=b._ymin;
	if (_xmax>b._xmax) _xmax=b._xmax;
	if (_ymax>b._ymax) _ymax=b._ymax;
	return *this;
}

box& box::extend_left(const box& b)
{
	_xmin-=b._xmax-b._xmin;
	if (_ymin>b._ymin) _ymin=b._ymin;
	if (_ymax<b._ymax) _ymax=b._ymax;
	return *this;
}

box& box::extend_right(const box& b)
{
	_xmax+=b._xmax-b._xmin;
	if (_ymin>b._ymin) _ymin=b._ymin;
	if (_ymax<b._ymax) _ymax=b._ymax;
	return *this;
}

box& box::extend_up(const box& b)
{
	_ymax+=b._ymax-b._ymin;
	if (_xmin>b._xmin) _xmin=b._xmin;
	if (_xmax<b._xmax) _xmax=b._xmax;
	return *this;
}

box& box::extend_down(const box& b)
{
	_ymin-=b._ymax-b._ymin;
	if (_xmin>b._xmin) _xmin=b._xmin;
	if (_xmax<b._xmax) _xmax=b._xmax;
	return *this;
}

box& box::extend_left(unsigned int xsize)
{
	_xmin-=xsize;
	return *this;
}

box& box::extend_right(unsigned int xsize)
{
	_xmax+=xsize;
	return *this;
}

box& box::extend_up(unsigned int ysize)
{
	_ymax+=ysize;
	return *this;
}

box& box::extend_down(unsigned int ysize)
{
	_ymin-=ysize;
	return *this;
}

box operator+(const box& b,const point& p)
{
	return box(
		b.xmin()+p.x(),
		b.ymin()+p.y(),
		b.xmax()+p.x(),
		b.ymax()+p.y());
}

box operator-(const box& b,const point& p)
{
	return box(
		b.xmin()-p.x(),
		b.ymin()-p.y(),
		b.xmax()-p.x(),
		b.ymax()-p.y());
}

box operator|(const box& b,const point& p)
{
	return box(
		min(b.xmin(),p.x()),
		min(b.ymin(),p.y()),
		max(b.xmax(),p.x()),
		max(b.ymax(),p.y()));
}

box operator+(const box& ba,const box& bb)
{
	return box(
		ba.xmin()+bb.xmin(),
		ba.ymin()+bb.ymin(),
		ba.xmax()+bb.xmax(),
		ba.ymax()+bb.ymax());
}

box operator-(const box& ba,const box& bb)
{
	return box(
		ba.xmin()-bb.xmin(),
		ba.ymin()-bb.ymin(),
		ba.xmax()-bb.xmax(),
		ba.ymax()-bb.ymax());
}

box operator|(const box& ba,const box& bb)
{
	return box(
		min(ba.xmin(),bb.xmin()),
		min(ba.ymin(),bb.ymin()),
		max(ba.xmax(),bb.xmax()),
		max(ba.ymax(),bb.ymax()));
}

box operator&(const box& ba,const box& bb)
{
	return box(
		max(ba.xmin(),bb.xmin()),
		max(ba.ymin(),bb.ymin()),
		min(ba.xmax(),bb.xmax()),
		min(ba.ymax(),bb.ymax()));
}

bool operator<=(const point& p,const box& b)
{
	return
		(p.x()>=b.xmin())&&
		(p.y()>=b.ymin())&&
		(p.x()<b.xmax())&&
		(p.y()<b.ymax());
}

bool operator==(const box& ba,const box& bb)
{
	return (
		(ba.xmin()^bb.xmin())|
		(ba.ymin()^bb.ymin())|
		(ba.xmax()^bb.xmax())|
		(ba.ymax()^bb.ymax()))==0;
}

bool operator!=(const box &ba,const box& bb)
{
	return (
		(ba.xmin()^bb.xmin())|
		(ba.ymin()^bb.ymin())|
		(ba.xmax()^bb.xmax())|
		(ba.ymax()^bb.ymax()))!=0;
}

ostream& operator<<(ostream& out,const box& b)
{
	out << '(' << b.xmin() << ',' << b.ymin() << ','
		<< b.xmax() << ',' << b.ymax() << ')';
	return out;
}

}; /* namespace graphics */
}; /* namespace rtk */
