// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_RAMFETCH
#define _RTK_EVENTS_RAMFETCH

#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

} /* namespace os */

namespace events {

/** A class to represent a RISC OS Message_RAMFetch user message. */
class ramfetch:
	public message
{
public:
	/** A mixin class for handling ramfetch events.
	 * If a class wishes to receive ramfetch events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle ramfetch event.
		 * @param ev the ramfetch event to be handled
		 */
		virtual void handle_event(ramfetch& ev)=0;
	};

	/** Construct ramfetch event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	ramfetch(desktop::component& target,int wimpcode,
		os::wimp_block& wimpblock);

	/** Destroy ramfetch event.
	 */
	virtual ~ramfetch();
protected:
	virtual bool deliver(desktop::component& dest);
public:
	/** Get data transfer buffer.
	 * @return the data transfer buffer
	 */
	void* buffer() const;

	/** Get size of the data transfer buffer.
	 * @return the size of the data transfer buffer
	 */
	unsigned int buffer_size() const;

	/** Send Message_RAMTransmit in reply to this event.
	 * @param count the number of bytes transferred
	 */
	void reply(unsigned int count) const;
};

} /* namespace events */
} /* namespace rtk */

#endif
