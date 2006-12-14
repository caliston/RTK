// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_KEY_PRESSED
#define _RTK_EVENTS_KEY_PRESSED

#include "rtk/events/event.h"

namespace rtk {
namespace os {

union wimp_block;

} /* namespace os */

namespace events {

/** A class to represent a RISC OS Key_Pressed event.
 */
class key_pressed:
	public event
{
public:
	/** A mixin class for handling key_pressed events.
	 * If a class wishes to receive key_pressed events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 *
	 * The processed flag is initially false.  If it remains false after
	 * all handlers have been called, Wimp_ProcessKey() will be called
	 * so that the keypress can be seen by other tasks.  (This action is
	 * performed automatically by the destructor.)
	 */
	class handler
	{
	public:
		/** Handle key_pressed event.
		 * @param ev the key_pressed event to be handled
		 */
		virtual void handle_event(key_pressed& ev)=0;
	};
private:
	/** The character code. */
	int _code;

	/** The processed flag.
	 * True if the keypress has been processed, otherwise false.
	 */
	bool _processed;
public:
	/** Construct key_pressed event.
	 * @param target the target of the event
	 * @param code the character code for the key that was pressed
	 */
	key_pressed(desktop::component& target,int code);

	/** Construct key_pressed event from Wimp_Poll block.
	 * @param target the target of the event
	 * @param block a block of the type returned by Wimp_Poll
	 */
	key_pressed(desktop::component& target,os::wimp_block& block);

	/** Destroy key_pressed event.
	 */
	virtual ~key_pressed();

	/** Get character code.
	 * @return the character code for the key that was pressed
	 */
	int code()
		{ return _code; }

	/** Get processed flag.
	 * @return true if the keypress has been processed, otherwise false
	 */
	bool processed()
		{ return _processed; }

	/** Set processed flag.
	 * @param value true if the keypress has been processed, otherwise false
	 */
	void processed(bool value);
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
