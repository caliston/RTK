// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_ARROW_CLICK
#define _RTK_EVENTS_ARROW_CLICK

#include "rtk/events/mouse_click.h"

namespace rtk {
namespace events {

using rtk::graphics::point;

/** An event class to represent a mouse click on an adjuster_arrow. */
class arrow_click:
	public mouse_click
{
public:
	/** A mixin class for handling arrow_click events.
	 * If a class wishes to receive arrow_click events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle arrow_click event.
		 * @param ev the arrow_click event to be handled
		 */
		virtual void handle_event(arrow_click& ev)=0;
	};
private:
	/** The number of steps by which the value should change. */
	int _steps;
public:
	/** Construct arrow_click event.
	 * @param click the underlying mouse click
	 * @param steps the number of steps by which the value should change
	 *  (typically -1 or +1)
	 */
	arrow_click(const mouse_click& click,int steps);

	/** Destroy arrow_click event. */
	virtual ~arrow_click();

	/** Get number of steps by which value should change
	 * @return the number of steps (typically -1 or +1)
	 */
	int steps() const
		{ return _steps; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
