// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_PROG_INFO_DBOX
#define _RTK_DESKTOP_PROG_INFO_DBOX

#include "rtk/desktop/grid_layout.h"
#include "rtk/desktop/window.h"

namespace rtk {
namespace desktop {

/** A class to represent a program information dialogue box.
 * Fields are displayed from top to bottom in the order they were added.
 * The Style Guide recommends five: "Name", "Purpose", "Author", "Licence"
 * and "Version".
 */
class prog_info_dbox:
	public window
{
private:
	/** The class from which this one is derived. */
	typedef window inherited;

	/** A grid layout component used to hold the label and value icons.
	 */
	grid_layout _grid;
public:
	/** Construct program information dialogue box.
	 * Initially the box is unpopulated.
	 */
	prog_info_dbox();

	/** Destroy program information dialogue box. */
	virtual ~prog_info_dbox();

	/** Add field.
	 * @param name the name of the field
	 * @param value the value of the field
	 */
	prog_info_dbox& add(const string& name,const string& value);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
