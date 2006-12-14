// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/ramfetch.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

ramfetch::ramfetch(component& target,int wimpcode,os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

ramfetch::~ramfetch()
{}

void* ramfetch::buffer() const
{
	return reinterpret_cast<void*>(wimpblock().word[5]);
}

unsigned int ramfetch::buffer_size() const
{
	return wimpblock().word[6];
}

void ramfetch::reply(unsigned int count) const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_RAMTransmit,npos);
	block.word[6]=count;
	bool last=count!=buffer_size();
	int wimpcode=(last)?swi::User_Message:swi::User_Message_Recorded;
	os::Wimp_SendMessage(wimpcode,block,thandle(),0,0);
}

bool ramfetch::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
