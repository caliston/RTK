// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_DEFAULT_BUTTON
#define _RTK_DESKTOP_DEFAULT_BUTTON

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS default action button.
 * The only difference between this class and a generic icon is the
 * default values set by the constructor.
 */
class default_button:
	public icon
{
public:
	/** Construct default action button.
	 * By default a default action button:
	 * - has a validation string of "R6,3";
	 * - has a border;
	 * - is filled;
	 * - is horizontally centred;
	 * - is vertically centred;
	 * - has a foreground colour of 7;
	 * - has a background colour of 1; and
	 * - has a button type of 3.
	 * All other attributes are as for an icon.
	 */
	default_button();
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
