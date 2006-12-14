// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/os.h"

#include "rtk/desktop/text_window.h"

namespace rtk {
namespace desktop {

namespace {

/** The number of centiseconds between auto-scroll operations. */
const unsigned int auto_scroll_period=10;

} /* anonymous namespace */

text_window::text_window():
	_auto_scroll_time(0)
{
	_text_area.xbaseline(xbaseline_left);
	_text_area.ybaseline(ybaseline_top);
	add(_text_area);
	border(true);
	wb_colour(0);
}

void text_window::handle_event(events::null_reason& ev)
{
	// Compare current time with time since last auto-scroll operation.
	unsigned int time=0;
	os::OS_ReadMonotonicTime(&time);
	if (time-_auto_scroll_time>auto_scroll_period)
	{
		// If elapsed time >= scroll_period then perform auto-scroll,
		// and record the time at which it took place.
		_auto_scroll_time=time;

		// Get pointer coordinates.
		point offset;
		parent_application(offset);
		os::pointer_info_get block;
		os::Wimp_GetPointerInfo(block);
		point lpos=block.p-offset;

		// Initially assume that scroll distance is zero.
		// If pointer lies outside bounding box, in either dimension,
		// then scroll in the relevant direction(s) at a rate
		// proportional to the distance outside the bounding box.
		point diff;
		if (lpos.x()<bbox().xmin())
		{
			diff.x(bbox().xmin()-lpos.x());
		}
		if (lpos.x()>=bbox().xmax())
		{
			diff.x(bbox().xmax()-lpos.x());
		}
		if (lpos.y()<bbox().ymin())
		{
			diff.y(bbox().ymin()-lpos.y());
		}
		if (lpos.y()>=bbox().ymax())
		{
			diff.y(bbox().ymax()-lpos.y());
		}

		// Scroll if the required distance is non-zero.
		if (diff!=point())
		{
			_text_area.origin(_text_area.origin()+diff,true);
		}
	}
}

void text_window::handle_event(events::key_pressed& ev)
{
	// Handle character codes with special meanings.
	// Otherwise, ignore the character.
	bool processed=true;
	int code=ev.code();
	switch (code)
	{
	case 0x19e:handle_page_down(); break;
	case 0x19f:handle_page_up(); break;
	default:
		processed=false;
		break;
	}
	if (processed) ev.processed(true);
}

void text_window::handle_page_down()
{
	// The number of lines to move downwards is equal to
	// the height of the window divided by the height of
	// one line, less one if the result remains positive.
	int ysize=bbox().ysize();
	int diff=ysize/_text_area.line_height();
	if (diff>1) --diff;
	_text_area.handle_down_lines(diff);
}

void text_window::handle_page_up()
{
	// The number of lines to move upwards is equal to
	// the height of the window divided by the height of
	// one line, less one if the result remains positive.
	int ysize=bbox().ysize();
	int diff=ysize/_text_area.line_height();
	if (diff>1) --diff;
	_text_area.handle_up_lines(diff);
}

} /* namespace desktop */
} /* namespace rtk */
