// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_NULL_REASON
#define _RTK_EVENTS_NULL_REASON

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a RISC OS Null_Reason event.
 * A null_reason event occurs when there are no other events waiting to be
 * processed.
 */
class null_reason:
	public event
{
public:
	/** A mixin class for handling null_reason events.
	 * If a class wishes to receive null_reason events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle null_reason event.
		 * @param ev the null_reason event to be handled
		 */
		virtual void handle_event(null_reason& ev)=0;
	};

	/** Construct null_reason event.
	 * @param target the target of the event
	 */
	null_reason(desktop::component& target);

	/** Destroy null_reason event.
	 */
	virtual ~null_reason();
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
