// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_DISPLAY_FIELD
#define _RTK_DESKTOP_DISPLAY_FIELD

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS display field.
 * The only difference between this class and a generic icon is the
 * default values set by the constructor.
 */
class display_field:
	public icon
{
public:
	/** Construct display field.
	 * By default a display field:
	 * - has a validation string of "R2";
	 * - has a border;
	 * - is filled;
	 * - is horizontally centred;
	 * - is vertically centred;
	 * - has a foreground colour of 7; and
	 * - has a background colour of 1.
	 * All other attributes are as for an icon.
	 */
	display_field();
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
