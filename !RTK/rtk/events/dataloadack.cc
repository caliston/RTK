// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/desktop/application.h"
#include "rtk/events/dataloadack.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

dataloadack::dataloadack(component& target,int wimpcode,
	const os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

dataloadack::~dataloadack()
{}

int dataloadack::whandle() const
{
	return wimpblock().word[5];
}

int dataloadack::ihandle() const
{
	return wimpblock().word[6];
}

point dataloadack::position() const
{
	return point(wimpblock().word[7],wimpblock().word[8]);
}

unsigned int dataloadack::estsize() const
{
	return wimpblock().word[9];
}

unsigned int dataloadack::filetype() const
{
	return wimpblock().word[10];
}

string dataloadack::pathname() const
{
	return string(wimpblock().byte+44);
}

bool dataloadack::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

} /* namespace events */
} /* namespace rtk */
