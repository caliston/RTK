// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/help_request.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

help_request::help_request(component& target,os::wimp_block& wimpblock):
	event(target),
	_position(point(wimpblock.word[5],wimpblock.word[6])),
	_buttons(wimpblock.word[7]),
	_msg_ref(wimpblock.word[2]),
	_help_task(wimpblock.word[1])
{
	// Calculate origin of target with respect to origin of screen.
	point origin;
	target.parent_application(origin);

	// Translate pointer location from coordinate space of screen
	// to coordinate space of target.
	_position-=origin;
}

help_request::~help_request()
{}

bool help_request::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

void help_request::reply(const string& text) const
{
	os::wimp_block block;
	block.word[3]=_msg_ref;
	block.word[4]=swi::Message_HelpReply;
	unsigned int i=text.copy(block.byte+20,223);
	block.byte[20+i]=0;
	block.word[0]=20+((i+4)&~3);
	os::Wimp_SendMessage(swi::User_Message,block,_help_task,0,0);
}

}; /* namespace events */
}; /* namespace rtk */
