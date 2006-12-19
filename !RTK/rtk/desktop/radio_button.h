// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_RADIO_BUTTON
#define _RTK_DESKTOP_RADIO_BUTTON

#include "rtk/desktop/icon.h"
#include "rtk/events/mouse_click.h"

namespace rtk {
namespace desktop {

/** A class to represent a RISC OS radio button.
 * The only differences between this class and a generic icon are the
 * default values set by the constructor, and the behaviour on an adjust click.
 */
class radio_button:
	public icon,
	public events::mouse_click::handler
{
public:
	/** Construct radio button.
	 * By default a radio button:
	 * - has a validation string of "Sradiooff,radioon";
	 * - is vertically centred;
	 * - has a foreground colour of 7;
	 * - has a button type of 11.
	 * All other attributes are as for an icon.
	 */
	radio_button();

	/** Handle mouse clicks.
	 * This ensures that an adjust click cannot deselect the icon.
	 * @param ev the mouse click event
	 */
	virtual void handle_event(events::mouse_click& ev);
};

} /* namespace desktop */
} /* namespace rtk */

#endif
