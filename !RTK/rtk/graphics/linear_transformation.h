// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_LINEAR_TRANSFORMATION
#define _RTK_GRAPHICS_LINEAR_TRANSFORMATION

#include <iosfwd>

#include "rtk/graphics/point.h"

namespace rtk {
namespace graphics {

/** A class for representing a linear transformation.
 * A linear transformation can express an expansion, rotation or shear
 * with respect to the origin, or a combination of those operations.
 *
 * This implementation uses a fixed-point representation to specify
 * the transformation coefficients.  An integer x, if identified as
 * a fixed-point quantity, should be interpreted as x/scale.
 *
 * The internal representation of this class should not be changed
 * because it has been written to match the representation used by
 * RISC OS.
 */
class linear_transformation
{
private:
	/** The fraction of x which maps to x (fixed-point). */
	int _xx;

	/** The fraction of x which maps to y (fixed-point). */
	int _xy;

	/** The fraction of y which maps to x (fixed-point). */
	int _yx;

	/** The fraction of y which maps to y (fixed-point). */
	int _yy;
public:
	/** The fixed-point scale factor. */
	static const int scale=0x10000;
private:
	/** Multiply (fixed-point)
	 * @param x the multiplier
	 * @param y the multiplicand
	 * @return z the product
	 */
	inline static int mul(int x,int y);
public:
	/** Construct linear identity transformation. */
	linear_transformation():
		_xx(scale),
		_xy(0),
		_yx(0),
		_yy(scale)
		{}

	/** Construct linear transformation given components.
	 * @param xx the fraction of x which maps to x (fixed-point)
	 * @param xy the fraction of x which maps to y (fixed-point)
	 * @param yx the fraction of y which maps to x (fixed-point)
	 * @param yy the fraction of y which maps to y (fixed-point)
	 */
	linear_transformation(int xx,int xy,int yx,int yy):
		_xx(xx),
		_xy(xy),
		_yx(yx),
		_yy(yy)
		{}

	/** Get fraction of x which maps to x.
	 * @return the fraction of x which maps to x (fixed-point)
	 */
	int xx() const
		{ return _xx; }

	/** Get fraction of x which maps to y.
	 * @return the fraction of x which maps to y (fixed-point)
	 */
	int xy() const
		{ return _xy; }

	/** Get fraction of y which maps to x.
	 * @return the fraction of y which maps to x (fixed-point)
	 */
	int yx() const
		{ return _yx; }

	/** Get fraction of y which maps to y.
	 * @return the fraction of y which maps to y (fixed-point)
	 */
	int yy() const
		{ return _yy; }

	/** Apply this transformation to a point.
	 * @param p the point to which the transformation is to be applied
	 * @return the transformed point
	 */
	point operator()(const point& p) const;

	/** Apply this transformation to another transformation.
	 * @param t the transformation to which this is to be applied
	 * @return the transformed transformation
	 */
	linear_transformation operator()(const linear_transformation& t) const;
};

std::ostream& operator<<(std::ostream& out,const linear_transformation& t);

} /* namespace graphics */
} /* namespace rtk */

#endif
