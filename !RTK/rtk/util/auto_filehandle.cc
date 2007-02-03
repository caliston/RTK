// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/os.h"
#include "rtk/util/auto_filehandle.h"

namespace rtk {
namespace util {

auto_filehandle::auto_filehandle(int handle):
	_handle(handle)
{}

auto_filehandle::~auto_filehandle()
{
	rtk::os::OS_Find0(_handle);
}

} /* namespace util */
} /* namespace rtk */
