// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_DISCARD
#define _RTK_EVENTS_DISCARD

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a request to discard one or more documents.
 * It typically results from the user clicking on the discard button
 * in a dcs_dbox or a quit_dbox.
 */
class discard:
	public event
{
public:
	/** A mixin class for handling discard events.
	 * If a class wishes to receive discard events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle discard event.
		 * @param ev the discard event to be handled
		 */
		virtual void handle_event(discard& ev)=0;
	};

	/** Construct discard event.
	 * @param target the target of the event
	 */
	discard(desktop::component& target);

	/** Destroy discard event.
	 */
	virtual ~discard();
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
