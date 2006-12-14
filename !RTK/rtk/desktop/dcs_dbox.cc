// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>

#include "rtk/desktop/dcs_dbox.h"
#include "rtk/events/discard.h"
#include "rtk/events/save.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;

dcs_dbox::dcs_dbox()
{
	close_icon(false);

	_message.text("This file has been modified");
	_message.xbaseline(xbaseline_centre);

	_discard_button.text("Discard");
	_cancel_button.text("Cancel");
	_save_button.text("Save");

	_row.add(_discard_button);
	_row.add(_cancel_button);
	_row.add(_save_button);
	_row.xgap(96);

	_column.add(_message);
	_column.add(_row);
	_column.ygap(80);
	_column.margin(box(-64,-16,64,80));

	add(_column);
}

dcs_dbox::~dcs_dbox()
{
	remove();
}

void dcs_dbox::resize() const
{
	int width=0;
	width=max(width,_discard_button.content_box().xsize());
	width=max(width,_cancel_button.content_box().xsize());
	width=max(width,_save_button.content_box().xsize());
	point _discard_size(width+16+_discard_button.border_box().xsize(),0);
	_discard_button.min_size(_discard_size);
	point _cancel_size(width+16+_cancel_button.border_box().xsize(),0);
	_cancel_button.min_size(_cancel_size);
	point _save_size(width+16+_save_button.border_box().xsize(),0);
	_save_button.min_size(_save_size);
	inherited::resize();
}

void dcs_dbox::handle_event(rtk::events::mouse_click& ev)
{
	if (ev.target()==&_discard_button)
	{
		handle_discard();
	}
	if (ev.target()==&_cancel_button)
	{
		handle_cancel();
	}
	if (ev.target()==&_save_button)
	{
		handle_save();
	}
}

void dcs_dbox::handle_event(rtk::events::key_pressed& ev)
{
	switch (ev.code())
	{
	case 0x0d:
		handle_save();
		ev.processed(true);
		break;
	case 0x1b:
		handle_cancel();
		ev.processed();
		break;
	}
}

void dcs_dbox::handle_discard()
{
	rtk::events::discard ev(*this);
	ev.post();
}

void dcs_dbox::handle_cancel()
{
	remove();
}

void dcs_dbox::handle_save()
{
	rtk::events::save ev(*this);
	ev.post();
}

} /* namespace desktop */
} /* namespace rtk */
