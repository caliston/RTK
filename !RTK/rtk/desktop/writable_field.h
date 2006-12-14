// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_WRITABLE_FIELD
#define _RTK_DESKTOP_WRITABLE_FIELD

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS writable field.
 * The only difference between this class and a generic icon is the
 * default values set by the constructor.
 */
class writable_field:
	public icon
{
public:
	/** Construct writable field.
	 * By default a writable field:
	 * - has a validation string of "Ktar;Pptr_write";
	 * - has a border;
	 * - is filled;
	 * - is horizontally centred;
	 * - is vertically centred;
	 * - has a foreground colour of 7;
	 * - has a background colour of 0; and
	 * - has a button type of 14.
	 * All other attributes are as for an icon.
	 */
	writable_field();
};

} /* namespace desktop */
} /* namespace rtk */

#endif
