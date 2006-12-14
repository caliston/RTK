// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_WINDOW
#define _RTK_DESKTOP_WINDOW

#include "rtk/desktop/basic_window.h"

namespace rtk {
namespace desktop {

/** A class to represent a standard top-level RISC OS window. */
class window:
	public basic_window
{
private:
	/** The class from which this one is derived. */
	typedef basic_window inherited;
public:
	/** Construct window.
	 * By default a window:
	 * - is movable;
	 * - has a title bar;
	 * - has a back icon;
	 * - has a close icon;
	 * - does not have scroll bars;
	 * - does not have a toggle icon;
	 * - does not have an adjust icon;
	 * - has a border;
	 * - has a button type of 10;
	 * - has a title foreground colour of 7;
	 * - has a title background colour of 2;
	 * - has a work area foreground colour of 7; and
	 * - has a work area background colour of 1.
	 * @param size the required initial size
	 */
	window(point size=point(640,512));
};

} /* namespace desktop */
} /* namespace rtk */

#endif
