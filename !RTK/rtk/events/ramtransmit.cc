// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/ramtransmit.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

ramtransmit::ramtransmit(component& target,int wimpcode,
	os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

ramtransmit::~ramtransmit()
{}

void* ramtransmit::buffer() const
{
	return reinterpret_cast<void*>(wimpblock().word[5]);
}

unsigned int ramtransmit::buffer_size() const
{
	return wimpblock().word[6];
}

void ramtransmit::reply(void* buffer,unsigned int buffer_size) const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_RAMFetch,npos);
	block.word[5]=reinterpret_cast<int>(buffer);
	block.word[6]=buffer_size;
	os::Wimp_SendMessage(swi::User_Message_Recorded,block,thandle(),0,0);
}

bool ramtransmit::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
