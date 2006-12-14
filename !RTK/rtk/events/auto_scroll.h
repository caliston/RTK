// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_AUTO_SCROLL
#define _RTK_EVENTS_AUTO_SCROLL

#include "rtk/graphics/box.h"
#include "rtk/events/event.h"

namespace rtk {
namespace events {

using rtk::graphics::box;

/** An event class for requesting auto-scroll to make a given area visible.
 * Typically this event would be posted by the owner of the caret when it
 * has moved due to a key press or some other event that a containing
 * window may wish to scroll to accommodate.
 */
class auto_scroll:
	public event
{
private:
	/** The bounding box of the area to be made visible. */
	box _bbox;
public:
	/** A mixin class for handling auto-scroll events.
	 * If a class wishes to receive auto-scroll events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle auto-scroll event.
		 * @param ev the auto-scroll event to be handled
		 */
		virtual void handle_event(auto_scroll& ev)=0;
	};

	/** Construct auto-scroll event.
	 * @param target the target of the event
	 * @param bbox the bounding box of the area to be made visible
	 */
	auto_scroll(desktop::component& target,const box& bbox);

	/** Destroy auto-scroll event. */
	virtual ~auto_scroll();

	/** Get bounding box.
	 * @return the bounding box of the area to be made visible
	 */
	const box& bbox() const
		{ return _bbox; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
