// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/window.h"

namespace rtk {
namespace desktop {

window::window(point size):
	basic_window(size)
{
	movable(true);
	back_icon(true);
	close_icon(true);
	title_bar(true);
	border(true);
}

}; /* namespace desktop */
}; /* namespace rtk */
