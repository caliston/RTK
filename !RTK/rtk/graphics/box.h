// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_BOX
#define _RTK_GRAPHICS_BOX

#include <iosfwd>

#include "rtk/graphics/point.h"

namespace rtk {
namespace graphics {

/** A class for representing an axis-aligned box in a two-dimensional plane.
 * Minimum values for coordinates are inclusive, maximum values are
 * exclusive.  It is permissible, but not necessarily meaningful, for
 * the maximum coordinate to be less than the minimum coordinate.
 */
class box
{
private:
	/** The minimum x-coordinate. */
	int _xmin;

	/** The minimum y-coordinate. */
	int _ymin;

	/** The maximum x-coordinate. */
	int _xmax;

	/** The maximum y-coordinate. */
	int _ymax;
public:
	/** Construct box with default coordinates of (0,0,0,0). */
	box():
		_xmin(0),
		_ymin(0),
		_xmax(0),
		_ymax(0)
		{}

	/** Construct box given bottom-left and top-right corners. 
	 * @param min the bottom-left corner (minimum x- and y-coordinates)
	 * @param max the top-right corner (maximum x- and y-coordinates)
	 */
	box(const point& min,const point& max):
		_xmin(min.x()),
		_ymin(min.y()),
		_xmax(max.x()),
		_ymax(max.y())
		{}

	/** Construct box given minimum and maximum x- and y-coordinates.
	 * @param xmin the minimum x-coordinate
	 * @param ymin the minimum y-coordinate
	 * @param xmax the maximum x-coordinate
	 * @param ymax the maximum y-coordinate
	 */
	box(int xmin,int ymin,int xmax,int ymax):
		_xmin(xmin),
		_ymin(ymin),
		_xmax(xmax),
		_ymax(ymax)
		{}

	/** Get minimum x-coordinate.
	 * This is the position of the left side of the box.
	 * @return the minimum x-coordinate
	 */
	int xmin() const
		{ return _xmin; }

	/** Get minimum y-coordinate.
	 * This is the position of the lower side of the box.
	 * @return the minimum y-coordinate
	 */
	int ymin() const
		{ return _ymin; }

	/** Get maximum x-coordinate.
	 * This is the position of the right side of the box.
	 * @return the maximum x-coordinate
	 */
	int xmax() const
		{ return _xmax; }

	/** Get maximum y-coordinate.
	 * This is the position of the upper side of the box.
	 * @return the maximum y-coordinate
	 */
	int ymax() const
		{ return _ymax; }

	/** Get point with minimum x-coordinate and minimum y-coordinate.
	 * This is the bottom-left corner of the box.
	 * @return a point with the specified coordinates
	 */
	point xminymin() const
		{ return point(_xmin,_ymin); }

	/** Get point with minimum x-coordinate and maximum y-coordinate.
	 * This is the top-left corner of the box.
	 * @return a point with the specified coordinates
	 */
	point xminymax() const
		{ return point(_xmin,_ymax); }

	/** Get point with maximum x-coordinate and minimum y-coordinate.
	 * This is the bottom-right corner of the box.
	 * @return a point with the specified coordinates
	 */
	point xmaxymin() const
		{ return point(_xmax,_ymin); }

	/** Get point with maximum x-coordinate and maximum y-coordinate.
	 * This is the top-right corner of the box.
	 * @return a point with the specified coordinates
	 */
	point xmaxymax() const
		{ return point(_xmax,_ymax); }

	/** Set minimum x-coordinate.
	 * @param xmin the required minimum x-coordinate
	 * @return a reference to this
	 */
	box& xmin(int xmin)
		{ _xmin=xmin; return *this; }

	/** Set minimum y-coordinate.
	 * @param ymin the required minimum y-coordinate
	 * @return a reference to this
	 */
	box& ymin(int ymin)
		{ _ymin=ymin; return *this; }

	/** Set maximum x-coordinate.
	 * @param xmax the required maximum x-coordinate
	 * @return a reference to this
	 */
	box& xmax(int xmax)
		{ _xmax=xmax; return *this; }

	/** Set maximum y-coordinate.
	 * @param ymax the required maximum y-coordinate
	 * @return a reference to this
	 */
	box& ymax(int ymax)
		{ _ymax=ymax; return *this; }

	/** Set minimum x-coordinate and minimum y-coordinate.
	 * @param p a point with the required x- and y-coordinates
	 * @return a reference to this
	 */
	box& xminymin(const point& p)
		{ _xmin=p.x(); _ymin=p.y(); return *this; }

	/** Set minimum x-coordinate and maximum y-coordinate.
	 * @param p a point with the required x- and y-coordinates
	 * @return a reference to this
	 */
	box& set_xminymax(const point& p)
		{ _xmin=p.x(); _ymax=p.y(); return *this; }

	/** Set maximum x-coordinate and minimum y-coordinate.
	 * @param p a point with the required x- and y-coordinates
	 * @return a reference to this
	 */
	box& set_xmaxymin(const point& p)
		{ _xmax=p.x(); _ymin=p.y(); return *this; }

	/** Set maximum x-coordinate and maximum y-coordinate.
	 * @param p a point with the required x- and y-coordinates
	 * @return a reference to this
	 */
	box& set_xmaxymax(const point& p)
		{ _xmax=p.x(); _ymax=p.y(); return *this; }

	/** Get size of box in x- and y-directions.
	 * @return a point with its x- and y-coordinates equal to the
	 *  width and height of the box respectively.
	 */
	point size() const
		{ return point(_xmax-_xmin,_ymax-_ymin); }

	/** Get size of box in x-direction.
	 * @return the size of the box in the x-direction
	 */
	int xsize() const
		{ return _xmax-_xmin; }

	/** Get size of box in y-direction.
	 * @return the size of the box in the y-direction
	 */
	int ysize() const
		{ return _ymax-_ymin; }

	/** Add components of a point.
	 * This operation can be used to move a box by an offset without
	 * changing its size.  The result overwrites the existing value of
	 * this.
	 * @param p the augend (a point)
	 * @return a reference to this
	 */
	box& operator+=(const point& p);

	/** Subtract components of a point.
	 * This operation can be used to move a box by the reverse of an
	 * offset without changing its size.  The result overwrites the
	 * existing value of this.
	 * @param p the subtrahend (a point)
	 * @return a reference to this
	 */
	box& operator-=(const point& p);

	/** Calculate union with a point.
	 * The result is the smallest box that contains both this box and
	 * the point.  It overwrites the existing value of this.
	 * @param p the point
	 * @return a reference to this
	 */
	box& operator|=(const point& p);

	/** Add components of another box.
	 * This operation can be used to place a border around a box.
	 * For example, adding box(-1,-1,1,1) would expand the addend by
	 * one unit in every direction.  The result overwrites the existing
	 * value of this.
	 * @param b the augend (a box)
	 * @return a reference to this
	 */
	box& operator+=(const box& b);

	/** Subtract components of another box.
	 * This operation can be used to remove a border from around a box.
	 * For example, subtracting box(-1,-1,1,1) would reduce the minuend by
	 * one unit in every direction.  The result overwrites the existing
	 * value of this.
	 * @param b the subtrahend (a box)
	 * @return a reference to this
	 */
	box& operator-=(const box& b);

	/** Calculate union with another box.
	 * The result is the smallest box that contains both arguments.
	 * It overwrites the existing value of this.
	 * @param b the other box
	 * @return a reference to this
	 */
	box& operator|=(const box& b);

	/** Calculate intersection with another box.
	 * The result is the largest box that is contained both by both
	 * arguments.  It overwrites the existing value of this.
	 * If the two boxes do not intersect then the result will have
	 * zero or negative height and/or width.
	 * @param b the other box
	 * @return a reference to this
	 */
	box& operator&=(const box& b);

	/** Extend this box by attaching another box to its left side.
	 * This box is extended left by the width of b.  It is also
	 * extended upward and/or downward so that any y-coordinate
	 * contained within b is also contained within the result.
	 * @param b the extension box
	 * @return a reference to this
	 */
	box& extend_left(const box& b);

	/** Extend this box by attaching another box to its right side.
	 * This box is extended right by the width of b.  It is also
	 * extended upward and/or downward so that any y-coordinate
	 * contained within b is also contained within the result.
	 * @param b the extension box
	 * @return a reference to this
	 */
	box& extend_right(const box& b);

	/** Extend this box by attaching another box to its upper side.
	 * This box is extended upward by the height of b.  It is also
	 * extended left and/or right so that any x-coordinate
	 * contained within b is also contained within the result.
	 * @param b the extension box
	 * @return a reference to this
	 */
	box& extend_up(const box& b);

	/** Extend this box by attaching another box to its lower side.
	 * This box is extended downward by the height of b.  It is also
	 * extended left and/or right so that any x-coordinate
	 * contained within b is also contained within the result.
	 * @param b the extension box
	 * @return a reference to this
	 */
	box& extend_down(const box& b);

	/** Extend this box to the left.
	 * This box is extended left by xsize.
	 * @param xsize the distance by which to extend
	 * @return a reference to this
	 */
	box& extend_left(unsigned int xsize);

	/** Extend this box to the right.
	 * @param xsize the distance by which to extend
	 * @return a reference to this
	 */
	box& extend_right(unsigned int xsize);

	/** Extend this box upwards.
	 * @param ysize the distance by which to extend
	 * @return a reference to this
	 */
	box& extend_up(unsigned int ysize);

	/** Extend this box downwards.
	 * @param ysize the distance by which to extend
	 * @return a reference to this
	 */
	box& extend_down(unsigned int ysize);
};

/** Add components of point to box.
 * This operation can be used to move a box by an offset without changing
 * its size.  The result is such that
 * box(ax,ay,bx,by)+point(cx,cy)==box(ax+cx,ay+cy,bx+cx,by+cy).
 * @param b the addend (a box)
 * @param p the augend (a point)
 * @return the sum
 */
box operator+(const box& b,const point& p);

/** Subtract components of point from box.
 * This operation can be used to move a box by the reverse of an offset
 * without changing its size.  The result is such that
 * box(ax,ay,bx,by)-point(cx,cy)==box(ax-cx,ay-cy,bx-cx,by-cy).
 * @param b the minuend (a box)
 * @param p the subtrahend (a point)
 * @return the remainder
 */
box operator-(const box& b,const point& p);

/** Calculate union of box and point.
 * The result is the smallest box that contains both arguments.
 * @param b the box
 * @param p the point
 * @return the union
 */
box operator|(const box& b,const point& p);

/** Add components of one box to another.
 * This operation can be used to place a border around a box.
 * For example, adding box(-1,-1,1,1) would expand the addend by
 * one unit in every direction.  The result is such that
 * box(ax,ay,bx,by)+box(cx,cy,dx,dy)==box(ax+cx,ay+cy,bx+dx,by+dy).
 * @param ba the addend (a box)
 * @param bb the augend (another box)
 * @return the sum
 */
box operator+(const box& ba,const box& bb);

/** Subtract components of one box to another.
 * This operation can be used to remove a border from around a box.
 * For example, subtracting box(-1,-1,1,1) would reduce the minuend by
 * one unit in every direction.  The result is such that
 * box(ax,ay,bx,by)-box(cx,cy,dx,dy)==box(ax-cx,ay-cy,bx-dx,by-dy).
 * @param ba the minuend (a box)
 * @param bb the subtrahend (another box)
 * @return the difference
 */
box operator-(const box& ba,const box& bb);

/** Calculate union of two boxes.
 * The result is the smallest box that contains both arguments.
 * @param ba the first box
 * @param bb the second box
 * @return the union
 */
box operator|(const box& ba,const box& bb);

/** Calculate intersection of two boxes.
 * The result is the largest box contained within both arguments.
 * If the arguments do not intersect then the result will have
 * zero or negative height and/or width.
 * @param ba the first box
 * @param bb the second box
 * @return the intersection
 */
box operator&(const box& ba,const box& bb);

/** Test whether a point is inside a box.
 * To be inside the box the coordinates of the point must be strictly
 * less than the maximum coordinates of the box, but not less than
 * the minimum coordinates.
 * @param p the point
 * @param b the box
 * @return true if point is inside, otherwise false
 */
bool operator<=(const point& p,const box& b);

/** Test whether two boxes coincide.
 * @param ba the first box
 * @param bb the second box
 * @return true if the boxes coincide, otherwise false
 */
bool operator==(const box& ba,const box& bb);

/** Test whether two boxes differ.
 * @param ba the first box
 * @param bb the second box
 * @return true if the boxes differ, otherwise false
 */
bool operator!=(const box& ba,const box& bb);

/** Write box to output stream.
 * The output is human-readable and of the form (xmin,ymin,xmax,ymax).
 * It uses whatever formatting state is applicable at the time.
 * @param out the output stream
 * @param b the box to be written
 * @return a reference to the output stream
 */
std::ostream& operator<<(std::ostream& out,const box& b);

}; /* namespace graphics */
}; /* namespace rtk */

#endif
