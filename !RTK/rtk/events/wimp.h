// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_WIMP
#define _RTK_EVENTS_WIMP

#include "rtk/events/event.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

/** A class to represent a RISC OS Wimp event.
 */
class wimp:
	public event
{
private:
	/** The Wimp event code. */
	int _wimpcode;
	/** The Wimp event block. */
	os::wimp_block* _wimpblock;
public:
	/** A mixin class for handling Wimp events.
	 * If a class wishes to receive Wimp events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle Wimp event.
		 * @param ev the Wimp event to be handled
		 */
		virtual bool handle_event(wimp& ev)=0;
	};

	/** Construct Wimp event.
	 * The event block is not copied, and must remain in existance
	 * for the lifetime of this object.  It is not freed when this
	 * object is destroyed.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	wimp(desktop::component& target,int wimpcode,os::wimp_block& wimpblock);

	/** Destroy Wimp event.
	 */
	virtual ~wimp();

	/** Get event code.
	 * @return the event code
	 */
	int event_code();

	/** Get event data.
	 * The value returned is a reference to the original wimp block
	 * passed to the constructor.  It can be expected to remain valid
	 * for at least the lifetime of this object.
	 * @return the event data.
	 */
	const os::wimp_block& event_data();
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
