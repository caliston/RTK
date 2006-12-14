// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_HELP_REQUEST
#define _RTK_EVENTS_HELP_REQUEST

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/event.h"

namespace rtk {
namespace os {

union wimp_block;

} /* namespace os */

namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Message_HelpRequest event.
 * The mouse button state is a bitmap:
 * - bit 0 = Adjust,
 * - bit 1 = Menu,
 * - bit 2 = Select.
 */
class help_request:
	public event
{
public:
	/** A mixin class for handling help_request events.
	 * If a class wishes to receive help_request events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle help_request event.
		 * @param ev the help_request event to be handled
		 */
		virtual void handle_event(help_request& ev)=0;
	};
private:
	/** The position of the mouse pointer. */
	graphics::point _position;

	/** The mouse button state. */
	int _buttons;

	/** The shift state. */
	int _shift;

	/** The message reference. */
	int _msg_ref;

	/** The help application task handle. */
	int _help_task;
public:
	/** Construct help_request event from wimp block.
	 * The mouse position and mouse button state are taken from the
	 * wimp block, the shift state is currently unimplemented.
	 * @param target the target of the event
	 * @param block the wimp block
	 */
	help_request(desktop::component& target,os::wimp_block& block);

	/** Destroy help_request event. */
	virtual ~help_request();

	/** Get position of mouse pointer.
	 * @return the position of the mouse pointer, with respect to
	 *  the target
	 */
	const point& position() const
		{ return _position; }

	/** Get mouse button state.
	 * @return the mouse button state
	 */
	int buttons() const
		{ return _buttons; }

	/** Send reply.
	 * @param text the help text to be displayed.
	 */
	void reply(const string& text) const;
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
