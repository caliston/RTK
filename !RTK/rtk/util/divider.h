// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_DIVIDER
#define _RTK_UTIL_DIVIDER

namespace rtk {
namespace util {

/** A class for dividing an integer into weighted fractions.
 * The class is initialised with a dividend and a divisor, then operator()
 * is called once for each fraction.  Normally the sum of the weights
 * should be equal to the divisor, in which case the sum of the returned
 * fractions will be equal to the dividend.
 * (Interpolation is performed using Bresenham's algorithm.)
 */
class divider
{
private:
	/** The dividend. */
	unsigned int _dividend;
	/** The divisor. */
	unsigned int _divisor;
	/** The accumulated error. */
	unsigned int _accumulator;
public:
	/** Construct a divider object.
	 * @param dividend the number to be divided
	 * @param divisor the number by which to divide
	 */
	divider(unsigned int dividend,unsigned int divisor);

	/** Return a weighted fraction of the dividend.
	 * The value returned is an integer.  On average, it is equal to
	 * the dividend multiplied by the weight divided by the divisor.
	 * @param weight the weight of this fraction
	 */
	inline unsigned int operator()(unsigned int weight);
};

inline unsigned int divider::operator()(unsigned int weight)
{
	_accumulator+=_dividend*weight;
	unsigned int quoitant=_accumulator/_divisor;
	_accumulator%=_divisor;
	return quoitant;
}

}; /* namespace util */
}; /* namespace rtk */

#endif
