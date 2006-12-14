// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_MENUSDELETED
#define _RTK_EVENTS_MENUSDELETED

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a RISC OS Message_MenusDeleted user message.
 */
class menusdeleted:
	public event
{
public:
	/** A mixin class for handling menusdeleted events.
	 * If a class wishes to receive menusdeleted events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle menusdeleted event.
		 * @param ev the menusdeleted event to be handled
		 */
		virtual void handle_event(menusdeleted& ev)=0;
	};

	/** Construct menusdeleted event.
	 * @param target the target of the event
	 */
	menusdeleted(desktop::component& target);

	/** Destroy menusdeleted event.
	 */
	virtual ~menusdeleted();
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
