// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_LOADED
#define _RTK_EVENTS_LOADED

#include <string>

#include "rtk/events/event.h"

namespace rtk {
namespace transfer {

class basic_load;

}; /* namespace transfer */

namespace events {

/** An event class for reporting that a file has been loaded. */
class loaded:
	public event
{
private:
	/** The load operation that loaded the file. */
	transfer::basic_load* _loadop;
public:
	/** A mixin class for handling loaded events.
	 * If a class wishes to receive loaded events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle loaded event.
		 * @param ev the loaded event to be handled
		 */
		virtual void handle_event(loaded& ev)=0;
	};

	/** Construct loaded event.
	 * @param target the target of the event
	 * @param loadop the load operation that loaded the file
	 */
	loaded(desktop::component& target,transfer::basic_load& loadop);

	/** Destroy loaded event. */
	virtual ~loaded();

	/** Get load operation.
	 * @return the load operation that loaded the file
	 */
	transfer::basic_load& loadop()
		{ return *_loadop; }
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
