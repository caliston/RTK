// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/message.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

message::message(component& target,int wimpcode,os::wimp_block& wimpblock):
	event(target),
	_wimpcode(wimpcode),
	_wimpblock(&wimpblock)
{}

message::~message()
{}

int message::thandle() const
{
	return _wimpblock->word[1];
}

int message::msgcode() const
{
	return _wimpblock->word[4];
}

void message::prepare_reply(os::wimp_block& reply,int msgcode,
	size_type size) const
{
	if (size==npos) size=_wimpblock->word[0];
	unsigned int words=(size+3)>>2;
	for (unsigned int i=5;i<words;++i) reply.word[i]=_wimpblock->word[i];
	reply.word[0]=size;
	reply.word[1]=0;
	reply.word[2]=0;
	reply.word[3]=_wimpblock->word[2];
	reply.word[4]=msgcode;
}

bool message::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
