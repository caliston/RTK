// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_CLOSE_WINDOW
#define _RTK_EVENTS_CLOSE_WINDOW

#include "rtk/events/event.h"

namespace rtk {
namespace desktop {

class basic_window;

}; /* namespace desktop */

namespace events {

/** A class to represent a RISC OS Close_Window_Request event.
 */
class close_window:
	public event
{
public:
	/** A mixin class for handling close_window events.
	 * If a class wishes to receive close_window events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle close_window event.
		 * @param ev the close_window event to be handled
		 */
		virtual void handle_event(close_window& ev)=0;
	};

	/** Construct close_window event.
	 * @param target the target of the event (the window to be closed)
	 */
	close_window(desktop::basic_window& target);

	/** Destroy close_window event.
	 */
	virtual ~close_window();

	/** Get target.
	 * @return the target of the event (the window to be closed)
	 */
	desktop::basic_window* target() const
		{ return (desktop::basic_window*)event::target(); }
protected:
	virtual bool deliver(desktop::component& dest);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
