// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_MENUWARNING
#define _RTK_EVENTS_MENUWARNING

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a RISC OS Message_MenuWarning user message.
 */
class menuwarning:
	public event
{
public:
	/** A mixin class for handling menuwarning events.
	 * If a class wishes to receive menuwarning events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle menuwarning event.
		 * @param ev the menuwarning event to be handled
		 */
		virtual void handle_event(menuwarning& ev)=0;
	};

	/** Construct menuwarning event.
	 * @param target the target of the event
	 */
	menuwarning(desktop::component& target);

	/** Destroy menuwarning event.
	 */
	virtual ~menuwarning();
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
