// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_POINT
#define _RTK_GRAPHICS_POINT

#include <iosfwd>

namespace rtk {
namespace graphics {

/** A class for representing a point in a two-dimensional plane.
 * Coordinates are signed integers.  The x-axis runs from left to
 * right and the y-axis runs from bottom to top. Overflow is not
 * detected and its effects are undefined.  The origin and scale
 * of the coordinate system are context-dependent.
 */
class point
{
private:
	/** The x-coordinate. */
	int _x;
	/** The y-coordinate. */
	int _y;
public:
	/** Construct point with default coordinates of (0,0). */
	point():
		_x(0),
		_y(0)
		{}

	/** Construct point from x- and y-coordinates.
	 * @param x the required x-coordinate
	 * @param y the required y-coordinate
	 */
	point(int x,int y):
		_x(x),
		_y(y)
		{}

	/** Get x-coordinate.
	 * @return the x-coordinate
	 */
	int x() const
		{ return _x; }

	/** Get y-coordinate.
	 * @return the y-coordinate
	 */
	int y() const
		{ return _y; }

	/** Set x-coordinate.
	 * @param x the required x-coordinate
	 */
	point& x(int x)
		{ _x=x; return *this; }

	/** Set y-coordinate.
	 * @param y the required y-coordinate
	 */
	point& y(int y)
		{ _y=y; return *this; }

	/** Add components of another point.
	 * The effect of pa+=pb is the same as pa=pa+pb.
	 * @param p the augend
	 * @return a reference to this point
	 */
	point& operator+=(const point& p)
		{ _x+=p._x; _y+=p._y; return *this; }

	/** Subtract components of another point.
	 * The effect of pa-=pb is the same as pa=pa-pb.
	 * @param p the subtrahend
	 * @return a reference to this point
	 */
	point& operator-=(const point& p)
		{ _x-=p._x; _y-=p._y; return *this; }
};

/** Negate components of a point.
 * The result is such that -point(x,y)==point(-x,-y).
 * @param p the point to be negated
 * @return a point with equal but opposite coordinates
 */
inline point operator-(const point& p)
{
	return point(-p.x(),-p.y());
}

/** Add components of two points.
 * The result is such that point(ax,ay)+point(bx,by)==point(ax+bx,ay+by).
 * @param pa the addend
 * @param pb the augend
 * @return the sum
 */
inline point operator+(const point& pa,const point& pb)
{
	return point(pa.x()+pb.x(),pa.y()+pb.y());
}

/** Subtract components of two points.
 * The result is such that point(ax,ay)-point(bx,by)==point(ax-bx,ay-by).
 * @param pa the minuend
 * @param pb the subtrahend
 * @return the remainder
 */
inline point operator-(const point& pa,const point& pb)
{
	return point(pa.x()-pb.x(),pa.y()-pb.y());
}

/** Test whether two points coincide.
 * @param pa the first point
 * @param pb the second point
 * @return true if the points coincide, otherwise false
 */
inline bool operator==(const point& pa,const point& pb)
{
	return (pa.x()==pb.x())&&(pa.y()==pb.y());
}

/** Test whether two points differ.
 * @param pa the first point
 * @param pb the second point
 * @return true if the points differ, otherwise false
 */
inline bool operator!=(const point& pa,const point& pb)
{
	return (pa.x()!=pb.x())||(pa.y()!=pb.y());
}

/** Write point to output stream.
 * The output is human-readable and of the form (x,y).
 * It uses whatever formatting state is applicable at the time.
 * @param out the output stream
 * @param p the point to be written
 * @return a reference to the output stream
 */
std::ostream& operator<<(std::ostream& out,const point& p);

} /* namespace graphics */
} /* namespace rtk */

#endif
