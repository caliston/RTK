// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/util/divider.h"

namespace rtk {
namespace util {

divider::divider(unsigned int dividend,unsigned int divisor):
	_dividend(dividend),
	_divisor(divisor),
	_accumulator(0)
{
	if (_divisor==0)
	{
		_dividend=0;
		_divisor=1;
	}
}

}; /* namespace util */
}; /* namespace rtk */
