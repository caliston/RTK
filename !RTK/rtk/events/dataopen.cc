// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/dataopen.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

dataopen::dataopen(component& target,int wimpcode,os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

dataopen::~dataopen()
{}

bool dataopen::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

int dataopen::whandle() const
{
	return wimpblock().word[5];
}

point dataopen::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

unsigned int dataopen::filetype() const
{
	return wimpblock().word[10];
}

string dataopen::pathname() const
{
	return string(wimpblock().byte+44);
}

void dataopen::reply() const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_DataLoadAck,npos);
	os::Wimp_SendMessage(swi::User_Message,block,thandle(),0,0);
}

}; /* namespace events */
}; /* namespace rtk */
