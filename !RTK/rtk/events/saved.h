// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_SAVED
#define _RTK_EVENTS_SAVED

#include <string>

#include "rtk/events/event.h"

namespace rtk {
namespace transfer {

class save;

} /* namespace transfer */

namespace events {

/** An event class for reporting that a file has been saved. */
class saved:
	public event
{
private:
	/** The save operation that saved the file. */
	transfer::save* _saveop;
public:
	/** A mixin class for handling saved events.
	 * If a class wishes to receive saved events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle saved event.
		 * @param ev the saved event to be handled
		 */
		virtual void handle_event(saved& ev)=0;
	};

	/** Construct saved event.
	 * @param target the target of the event
	 * @param saveop the saved operation that saved the file
	 */
	saved(desktop::component& target,transfer::save& saveop);

	/** Destroy saved event. */
	virtual ~saved();

	/** Get save operation.
	 * @return the save operation that saved the file
	 */
	transfer::save& saveop()
		{ return *_saveop; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
