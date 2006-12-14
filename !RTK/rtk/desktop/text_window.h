// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_TEXT_WINDOW
#define _RTK_DESKTOP_TEXT_WINDOW

#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/text_area.h"

#include "rtk/events/null_reason.h"
#include "rtk/events/key_pressed.h"

namespace rtk {
namespace desktop {

/** A class consisting of a basic_window with an embedded text_area.
 * The window implements functionality, such as automatic scrolling
 * during a drag operation, which the text_area would not easily be
 * able to provide by itself.
 */
class text_window:
	public basic_window,
	public events::null_reason::handler,
	public events::key_pressed::handler
{
private:
	/** The embedded text area. */
	desktop::text_area _text_area;

	/** The most recent monotonic time when automatic scrolling occurred. */
	unsigned int _auto_scroll_time;
public:
	/** Construct text window.
	 * By default the window:
	 * - has a border; and
	 * - has a work-area background colour of 0.
	 *
	 * The embedded text area is automatically added to the window.
	 */
	text_window();

	virtual void handle_event(events::null_reason& ev);
	virtual void handle_event(events::key_pressed& ev);

	/** Get embedded text area.
	 * @return a reference to the embedded text area
	 */
	desktop::text_area& text_area()
		{ return _text_area; }

	/** Move caret one page downwards. */
	void handle_page_down();

	/** Move caret one page upwards. */
	void handle_page_up();
};

} /* namespace desktop */
} /* namespace rtk */

#endif
