// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/util/refcount.h"

namespace rtk {
namespace util {

refcount::refcount():
	_count(0)
{}

refcount::~refcount()
{}

void refcount::inc_count()
{
	++_count;
}

void refcount::dec_count()
{
	if (!--_count) delete this;
}

}; /* namespace util */
}; /* namespace rtk */
