// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_MESSAGE
#define _RTK_EVENTS_MESSAGE

#include "rtk/events/event.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

/** A class to represent a RISC OS user message.
 * This can be either a User_Message or a User_Message_Recorded.
 */
class message:
	public event
{
private:
	/** The Wimp event block. */
	os::wimp_block* _wimpblock;
public:
	/** A mixin class for handling message events.
	 * If a class wishes to receive message events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle message event.
		 * @param ev the message event to be handled
		 */
		virtual void handle_event(message& ev)=0;
	};

	/** Construct message event.
	 * The message block is not copied, and must remain in existance
	 * for the lifetime of this object.  It is not freed when this
	 * object is destroyed.
	 * @param target the target of the event
	 * @param wimpblock the Wimp event block
	 */
	message(desktop::component& target,os::wimp_block& wimpblock);

	/** Destroy message event.
	 */
	virtual ~message();

	/** Get message action code.
	 * This is equal to message_data().word[4].
	 * @return the message action code
	 */
	int message_action_code();

	/** Get message data.
	 * The value returned is a reference to the original message block
	 * passed to the constructor.  It can be expected to remain valid
	 * for at least the lifetime of this object.
	 * @return the message data
	 */
	const os::wimp_block& data();
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
