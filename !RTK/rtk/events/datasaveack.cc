// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/datasaveack.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

datasaveack::datasaveack(component& target,int wimpcode,
	os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

datasaveack::~datasaveack()
{}

int datasaveack::whandle() const
{
	return wimpblock().word[5];
}

int datasaveack::ihandle() const
{
	return wimpblock().word[6];
}

point datasaveack::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

unsigned int datasaveack::estsize() const
{
	return wimpblock().word[9];
}

unsigned int datasaveack::filetype() const
{
	return wimpblock().word[10];
}

string datasaveack::pathname() const
{
	return string(wimpblock().byte+44);
}

void datasaveack::reply() const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_DataLoad,npos);
	os::Wimp_SendMessage(swi::User_Message_Recorded,block,thandle(),0,0);
}

bool datasaveack::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
