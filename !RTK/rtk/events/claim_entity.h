// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_CLAIM_ENTITY
#define _RTK_EVENTS_CLAIM_ENTITY

#include "rtk/events/message.h"

namespace rtk {
namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Message_ClaimEntity user message. */
class claim_entity:
	public message
{
public:
	/** A mixin class for handling claim_entity events.
	 * If a class wishes to receive claim_entity events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle claim_entity event.
		 * @param ev the claim_entity event to be handled
		 */
		virtual void handle_event(claim_entity& ev)=0;
	};

	/** Construct claim_entity event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	claim_entity(desktop::component& target,int wimpcode,
		os::wimp_block& wimpblock);

	/** Destroy claim_entity event. */
	virtual ~claim_entity();
protected:
	virtual bool deliver(desktop::component& dest);
public:
	/** Test whether the caret or selection has been claimed.
	 * @return true if the caret or selection has been claimed
	 */
	bool claim_selection() const;

	/** Test whether the clipboard has been claimed.
	 * @return true if the clipboard has been claimed
	 */
	bool claim_clipboard() const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
