// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_ENTERING_WINDOW
#define _RTK_EVENTS_ENTERING_WINDOW

#include "rtk/events/event.h"

namespace rtk {
namespace desktop {

class basic_window;

} /* namespace desktop */

namespace events {

/** A class to represent a RISC OS Pointer_Entering_Window event.
 */
class entering_window:
	public event
{
public:
	/** A mixin class for handling entering_window events.
	 * If a class wishes to receive entering_window events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle entering_window event.
		 * @param ev the entering_window event to be handled
		 */
		virtual void handle_event(entering_window& ev)=0;
	};

	/** Construct entering_window event.
	 * @param target the target of the event (the window entered)
	 */
	entering_window(desktop::basic_window& target);

	/** Destroy entering_window event.
	 */
	virtual ~entering_window();

	/** Get target.
	 * @return the target of the event (the window entered)
	 */
	desktop::basic_window* target() const
		{ return (desktop::basic_window*)target(); }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
