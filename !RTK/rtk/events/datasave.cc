// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/datasave.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

datasave::datasave(component& target,int wimpcode,
	const os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

datasave::~datasave()
{}

int datasave::whandle() const
{
	return wimpblock().word[5];
}

int datasave::ihandle() const
{
	return wimpblock().word[6];
}

point datasave::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

unsigned int datasave::estsize() const
{
	return wimpblock().word[9];
}

unsigned int datasave::filetype() const
{
	return wimpblock().word[10];
}

string datasave::leafname() const
{
	return string(wimpblock().byte+44);
}

void datasave::reply(void* buffer,unsigned int buffer_size) const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_RAMFetch,28);
	block.word[5]=reinterpret_cast<int>(buffer);
	block.word[6]=buffer_size;
	os::Wimp_SendMessage(swi::User_Message_Recorded,block,thandle(),0,0);
}

void datasave::reply(const string& pathname,bool secure) const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_DataSaveAck,npos);
	if (!secure) block.word[9]=-1;
	unsigned int i=pathname.copy(block.byte+44,211);
	block.byte[44+i]=0;
	block.word[0]=44+((i+4)&~3);
	os::Wimp_SendMessage(swi::User_Message,block,thandle(),0,0);
}

bool datasave::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
