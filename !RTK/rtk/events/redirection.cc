// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/events/redirection.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

redirection::redirection():
	_redirect(0)
{}

redirection::~redirection()
{}

void redirection::redirect(component* redirect)
{
	_redirect=redirect;
}

}; /* namespace events */
}; /* namespace rtk */
