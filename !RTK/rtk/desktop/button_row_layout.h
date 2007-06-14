// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_BUTTON_ROW_LAYOUT
#define _RTK_DESKTOP_BUTTON_ROW_LAYOUT

#include <vector>

#include "rtk/desktop/basic_row_layout.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging buttons in a row.
 * It is possible to specify the gap to be placed between cells and the
 * margin to be placed around the layout as a whole. The layout
 * will expand to fill any extra space made available to it, ensuring
 * that each button is (excluding the icon border) an equal width.
 */
class button_row_layout:
	public basic_row_layout
{
private:
	mutable int _bordersize;
	mutable int _maxxsize;

public:
	/** Construct row layout.
	 * @param xcells the required number of cells (defaults to 0)
	 */
	button_row_layout(size_type xcells=0);

	virtual box auto_bbox() const;
	virtual void reformat(const point& origin,const box& pbbox);
};

} /* namespace desktop */
} /* namespace rtk */

#endif
