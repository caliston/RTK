// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/claim_entity.h"

namespace rtk {
namespace events {

using rtk::desktop::component;

claim_entity::claim_entity(component& target,int wimpcode,
	os::wimp_block& wimpblock):
	message(target,wimpcode,wimpblock)
{}

claim_entity::~claim_entity()
{}

bool claim_entity::claim_selection() const
{
	return wimpblock().word[5]&3;
}

bool claim_entity::claim_clipboard() const
{
	return wimpblock().word[5]&4;
}

bool claim_entity::deliver(component& dest)
{
	handler* h=dynamic_cast<handler*>(&dest);
	if (h) h->handle_event(*this);
	return h;
}

}; /* namespace events */
}; /* namespace rtk */
