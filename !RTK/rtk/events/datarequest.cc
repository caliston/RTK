// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/datarequest.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

datarequest::datarequest(component& target,int wimpcode,
	const os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

datarequest::~datarequest()
{}

bool datarequest::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

int datarequest::whandle() const
{
	return wimpblock().word[5];
}

int datarequest::ihandle() const
{
	return wimpblock().word[6];
}

point datarequest::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

int datarequest::flags() const
{
	return wimpblock().word[9];
}

bool datarequest::request_clipboard() const
{
	return wimpblock().word[9]&4;
}

vector<unsigned int> datarequest::filetypes() const
{
	unsigned int size=wimpblock().word[0]>>3;
	unsigned int n=0;
	while ((n+10<size)&&(wimpblock().word[n+10]!=-1)) ++n;
	return vector<unsigned int>(wimpblock().word+10,wimpblock().word+n+10);
}

void datarequest::reply(unsigned int filetype,unsigned int estsize) const
{
	os::wimp_block block;
	prepare_reply(block,swi::Message_DataSave,48);
	block.word[9]=estsize;
	block.word[10]=filetype;
	block.word[11]=0;
	os::Wimp_SendMessage(swi::User_Message,block,thandle(),0,0);
}

}; /* namespace events */
}; /* namespace rtk */
