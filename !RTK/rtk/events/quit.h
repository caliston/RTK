// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_QUIT
#define _RTK_EVENTS_QUIT

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a RISC OS Message_Quit user message.
 */
class quit:
	public event
{
public:
	/** A mixin class for handling quit events.
	 * If a class wishes to receive quit events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle quit event.
		 * @param ev the quit event to be handled
		 */
		virtual void handle_event(quit& ev)=0;
	};

	/** Construct quit event.
	 * @param target the target of the event
	 */
	quit(desktop::component& target);

	/** Destroy quit event.
	 */
	virtual ~quit();
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
