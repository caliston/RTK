// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_RAMTRANSMIT
#define _RTK_EVENTS_RAMTRANSMIT

#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

/** A class to represent a RISC OS Message_RAMTransmit user message. */
class ramtransmit:
	public message
{
public:
	/** A mixin class for handling ramtransmit events.
	 * If a class wishes to receive ramtransmit events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle ramtransmit event.
		 * @param ev the ramtransmit event to be handled
		 */
		virtual void handle_event(ramtransmit& ev)=0;
	};

	/** Construct ramtransmit event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	ramtransmit(desktop::component& target,int wimpcode,
		os::wimp_block& wimpblock);

	/** Destroy ramtransmit event.
	 */
	virtual ~ramtransmit();
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

	/** Send Message_RAMFetch in reply to this event.
	 * @param buffer the data transfer buffer
	 * @param buffer_size the size of the data transfer buffer
	 */
	void reply(void* buffer,unsigned int buffer_size) const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
