// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_INFO_DBOX
#define _RTK_DESKTOP_INFO_DBOX

#include "rtk/desktop/prog_info_dbox.h"

namespace rtk {
namespace desktop {

/** A class to represent a program information dialogue box.
 * @deprecated This class has been replaced by prog_info_dbox.
 */
class info_dbox:
	public prog_info_dbox
{
public:
	/** Construct program information dialogue box.
	 * Initially the box is unpopulated.
	 */
	info_dbox();

	/** Destroy program information dialogue box. */
	virtual ~info_dbox();
};

} /* namespace desktop */
} /* namespace rtk */

#endif
