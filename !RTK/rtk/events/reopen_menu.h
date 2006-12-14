// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_REOPEN_MENU
#define _RTK_EVENTS_REOPEN_MENU

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** An event class for use when the menu tree is to be reopened.
 * @todo This mechanism does not currently work for top-level dialogue boxes.
 */
class reopen_menu:
	public event
{
public:
	/** A mixin class for handling reopen_menu events.
	 * If a class wishes to receive reopen_menu events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle reopen_menu event.
		 * @param ev the reopen_menu event to be handled
		 */
		virtual void handle_event(reopen_menu& ev)=0;
	};

	/** Construct reopen_menu event.
	 * @param target the target of the event
	 */
	reopen_menu(desktop::component& target);

	/** Destroy reopen_menu event.
	 */
	virtual ~reopen_menu();
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
