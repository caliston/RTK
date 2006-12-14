// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_MOUSE_CLICK
#define _RTK_EVENTS_MOUSE_CLICK

#include "rtk/graphics/point.h"
#include "rtk/events/event.h"

namespace rtk {
namespace os {

union wimp_block;

} /* namespace os */

namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Mouse_Click event.
 * The mouse button state is a bitmap:
 * - bit 0 = Adjust,
 * - bit 1 = Menu,
 * - bit 2 = Select.
 */
class mouse_click:
	public event
{
public:
	/** A mixin class for handling mouse_click events.
	 * If a class wishes to receive mouse_click events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle mouse_click event.
		 * @param ev the mouse_click event to be handled
		 */
		virtual void handle_event(mouse_click& ev)=0;
	};
private:
	/** The position of the mouse pointer. */
	graphics::point _position;

	/** The mouse button state. */
	int _buttons;

	/** The shift state. */
	int _shift;
public:
	/** Construct mouse_click event.
	 * @param target the target of the event
	 * @param position the mouse position, with respect to the target
	 * @param buttons the mouse button state
	 * @param shift the shift state
	 */
	mouse_click(desktop::component& target,const point& position,
		int buttons,int shift);

	/** Construct mouse_click event from wimp block.
	 * The mouse position and mouse button state are taken from the
	 * wimp block, the shift state is currently unimplemented.
	 * @param target the target of the event
	 * @param block the wimp block
	 */
	mouse_click(desktop::component& target,os::wimp_block& block);

	/** Destroy mouse_click event. */
	virtual ~mouse_click();

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

	/** Get shift state.
	 * @return the shift state
	 */
	int shift() const
		{ return _shift; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
