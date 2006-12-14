// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_ACTION_BUTTON
#define _RTK_DESKTOP_ACTION_BUTTON

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS action button.
 * The only difference between this class and a generic icon is the
 * default values set by the constructor.
 */
class action_button:
	public icon
{
public:
	/** Construct action button.
	 * By default an action button:
	 * - has a validation string of "R5,3";
	 * - has a border;
	 * - is filled;
	 * - is horizontally centred;
	 * - is vertically centred;
	 * - has a foreground colour of 7;
	 * - has a background colour of 1; and
	 * - has a button type of 3.
	 * All other attributes are as for an icon.
	 */
	action_button();
};

} /* namespace desktop */
} /* namespace rtk */

#endif
