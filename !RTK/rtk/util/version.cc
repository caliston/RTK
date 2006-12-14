// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/util/version.h"

namespace rtk {
namespace util {

const unsigned int* version()
{
	static const unsigned int version[]={0,7,1};
	return version;
}

void version(const unsigned int* version)
{}

} /* namespace util */
} /* namespace rtk */
