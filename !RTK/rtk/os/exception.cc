// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <kernel.h>

#include "rtk/os/exception.h"

namespace rtk {
namespace os {

exception::exception(_kernel_oserror* err):
	_err(err)
{}

const char* exception::what() const throw()
{
	return _err->errmess;
}

} /* namespace os */
} /* namespace rtk */
