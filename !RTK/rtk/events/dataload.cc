// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/dataload.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

dataload::dataload(component& target,int wimpcode,
	const os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

dataload::~dataload()
{}

int dataload::whandle() const
{
	return wimpblock().word[5];
}

int dataload::ihandle() const
{
	return wimpblock().word[6];
}

point dataload::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

unsigned int dataload::estsize() const
{
	return wimpblock().word[9];
}

unsigned int dataload::filetype() const
{
	return wimpblock().word[10];
}

string dataload::pathname() const
{
	return string(wimpblock().byte+44);
}

void dataload::reply() const
{
	static os::wimp_block block;
	prepare_reply(block,swi::Message_DataLoadAck,npos);
	os::Wimp_SendMessage(swi::User_Message,block,thandle(),0,0);
}

bool dataload::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
