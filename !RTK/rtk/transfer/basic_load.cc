// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/message.h"
#include "rtk/events/datasaveack.h"
#include "rtk/events/ramfetch.h"
#include "rtk/events/ramtransmit.h"
#include "rtk/events/loaded.h"
#include "rtk/transfer/basic_load.h"

namespace rtk {
namespace transfer {

basic_load::basic_load():
	_thandle(0),
	_filetype(0xffd)
{
	os::Wimp_ReadSysInfo(5,&_thandle,0);
}

basic_load::~basic_load()
{}

graphics::box basic_load::min_bbox() const
{
	return graphics::box();
}

void basic_load::handle_event(events::datasave& ev)
{
	// A Message_DataSave is acted upon at any time.
	ev.reply(temp_pathname());
}

void basic_load::handle_event(events::dataload& ev)
{
	// A Message_DataLoad is acted upon at any time.
	put_file(ev.pathname(),ev.estsize());
	ev.reply();
	events::loaded ev2(*this,*this);
	ev2.post();
	remove();
}

void basic_load::handle_event(events::dataopen& ev)
{
	// A Message_DataOpen is acted upon at any time.
	// Unlike Message_DataLoad, the reply is sent before the file is loaded.
	ev.reply();
	put_file(ev.pathname(),0);
	events::loaded ev2(*this,*this);
	ev2.post();
	remove();
}

void basic_load::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpcode)
	{
	case 17:
	case 18:
	case 19:
		deliver_message(wimpcode,wimpblock);
		break;
	default:
		{
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void basic_load::deliver_message(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_RAMFetch:
		{
			events::ramfetch ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_RAMTransmit:
		{
			events::ramtransmit ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	default:
		{
			events::message ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

string basic_load::temp_pathname() const
{
	static const string pathname("<Wimp$Scrap>");
	return pathname;
}

} /* namespace transfer */
} /* namespace rtk */
