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
public:
	/** A type for representing byte counts. */
	typedef unsigned int size_type;

	/** A null value for use in place of a byte count. */
	static const size_type npos=static_cast<size_type>(-1);
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

	/** Get task handle of sender.
	 * This is equal to data().word[1].
	 * @return the task handle of the sender
	 */
	int thandle() const;

	/** Get message action code.
	 * This is equal to data().word[4].
	 * @return the message action code
	 */
	int msgcode() const;

	/** Get message data.
	 * The value returned is a reference to the original message block
	 * passed to the constructor.  It can be expected to remain valid
	 * for at least the lifetime of this object.
	 * @return the message data
	 */
	const os::wimp_block& data() const
		{ return *_wimpblock; }

	/** Prepare reply to message.
	 * @param reply the wimp block that will contain the reply
	 * @param msgcode the required message action code
	 * @param size the required reply length in bytes, or npos to preserve
	 */
	void prepare_reply(os::wimp_block& reply,int msgcode,size_type size) const;
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
