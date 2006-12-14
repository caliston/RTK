// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_USER_DRAG_BOX
#define _RTK_EVENTS_USER_DRAG_BOX

#include <string>

#include "rtk/graphics/box.h"
#include "rtk/events/event.h"

namespace rtk {
namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS User_Drag_Box event. */
class user_drag_box:
	public event
{
public:
	/** A mixin class for handling user_drag_box events.
	 * If a class wishes to receive user_drag_box events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle user_drag_box event.
		 * @param ev the user_drag_box event to be handled
		 */
		virtual void handle_event(user_drag_box& ev)=0;
	};
private:
	/** The final position of the drag box, with respect to the target. */
	graphics::box _dbox;
public:
	/** Construct user_drag_box event.
	 * @param target the target of the event
	 * @param dbox the final position of the drag box, with respect to
	 *  the target.
	 */
	user_drag_box(desktop::component& target,const graphics::box& dbox);

	/** Construct user_drag_box event from wimp block.
	 * @param target the target of the event
	 * @param block the wimp block
	 */
	user_drag_box(desktop::component& target,os::wimp_block& block);

	/** Destroy user_drag_box event. */
	virtual ~user_drag_box();

	/** Get final position of drag box.
	 * @return the final position of the drag box, with respect to
	 *  the target.
	 */
	graphics::box dbox() const
		{ return _dbox; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
