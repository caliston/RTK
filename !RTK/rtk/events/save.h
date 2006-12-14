// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_SAVE
#define _RTK_EVENTS_SAVE

#include "rtk/events/event.h"

namespace rtk {
namespace events {

/** A class to represent a request to save one or more documents.
 * It typically results from the user clicking on the save button
 * in a dcs_dbox.
 */
class save:
	public event
{
public:
	/** A mixin class for handling save events.
	 * If a class wishes to receive save events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle save event.
		 * @param ev the save event to be handled
		 */
		virtual void handle_event(save& ev)=0;
	};

	/** Construct save event.
	 * @param target the target of the event
	 */
	save(desktop::component& target);

	/** Destroy save event.
	 */
	virtual ~save();
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
