// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_MENU_SELECTION
#define _RTK_EVENTS_MENU_SELECTION

#include "rtk/events/event.h"

namespace rtk {
namespace desktop {

class menu_item;

}; /* namespace desktop; */

namespace events {

/** A class to represent a RISC OS Menu_Selection event.
 * <P>The mouse button state is a bitmap.  Bit 0 corresponds to Adjust,
 * bit 1 to Menu and bit 2 to Select.
 */
class menu_selection:
	public event
{
public:
	/** A mixin class for handling menu_selection events.
	 * If a class wishes to receive menu_selection events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle menu_selection event.
		 * @param ev the menu_selection event to be handled
		 */
		virtual void handle_event(menu_selection& ev)=0;
	};
private:
	int _buttons;
public:
	/** Construct menu_selection event.
	 * @param target the target of the event (the selected menu item)
	 * @param buttons the mouse button state
	 */
	menu_selection(desktop::menu_item& target,int buttons);

	/** Construct menu_selection event.
	 * The mouse button state is read using Wimp_GetPointerInfo.
	 * @param target the target of the event (the selected menu item)
	 */
	menu_selection(desktop::menu_item& target);

	/** Destroy menu_selection event.
	 */
	virtual ~menu_selection();

	/** Get mouse button state.
	 * @return the mouse button state
	 */
	int buttons() const
		{ return _buttons; }
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
