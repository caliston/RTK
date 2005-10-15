// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_ADJUSTER_ARROW
#define _RTK_DESKTOP_ADJUSTER_ARROW

#include "rtk/desktop/icon.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/arrow_click.h"
#include "rtk/events/redirection.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS adjuster arrow.
 * The arrow may be configured to point up, down, left or right, and
 * to increment or decrement an attached selection field.
 */
class adjuster_arrow:
	public icon,
	public events::mouse_click::handler,
	public events::redirection
{
public:
	enum arrow_type
	{
		arrow_left,
		arrow_right,
		arrow_down,
		arrow_up
	};
private:
	int _delta;
public:
	/** Construct adjuster arrow.
	 * By default a left or down arrow decrements and a right or up arrow
	 * increments.
	 * @param type the arrow type (left, right, down or up)
	 * @param delta the arrow direction (-1 or +1, the default of 0
	 *  meaning to choose automatically)
	 */
	explicit adjuster_arrow(arrow_type type,int delta=0);

	virtual void handle_event(events::mouse_click& ev);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
