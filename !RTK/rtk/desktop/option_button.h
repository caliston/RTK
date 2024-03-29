// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_OPTION_BUTTON
#define _RTK_DESKTOP_OPTION_BUTTON

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS option button.
 * The only difference between this class and a generic icon is the
 * default values set by the constructor.
 */
class option_button:
	public icon
{
public:
	/** Construct option button.
	 * By default an option button:
	 * - has a validation string of "Soptoff,opton";
	 * - is vertically centred;
	 * - has a foreground colour of 7;
	 * - has a button type of 11.
	 * All other attributes are as for an icon.
	 */
	option_button();
};

} /* namespace desktop */
} /* namespace rtk */

#endif
