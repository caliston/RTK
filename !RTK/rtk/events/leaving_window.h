// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_LEAVING_WINDOW
#define _RTK_EVENTS_LEAVING_WINDOW

#include "rtk/events/event.h"

namespace rtk {
namespace desktop {

class basic_window;

}; /* namespace desktop */

namespace events {

/** A class to represent a RISC OS Pointer_Leaving_Window event.
 */
class leaving_window:
	public event
{
public:
	/** A mixin class for handling leaving_window events.
	 * If a class wishes to receive leaving_window events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle leaving_window event.
		 * @param ev the leaving_window event to be handled
		 */
		virtual void handle_event(leaving_window& ev)=0;
	};

	/** Construct leaving_window event.
	 * @param target the target of the event (the window left)
	 */
	leaving_window(desktop::basic_window& target);

	/** Destroy leaving_window event.
	 */
	virtual ~leaving_window();

	/** Get target.
	 * @return the target of the event (the window left)
	 */
	desktop::basic_window* target() const
		{ return (desktop::basic_window*)target(); }
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
