// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_DATAOPEN
#define _RTK_EVENTS_DATAOPEN

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Message_DataOpen user message. */
class dataopen:
	public message
{
public:
	/** A mixin class for handling dataopen events.
	 * If a class wishes to receive dataopen events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle dataopen event.
		 * @param ev the dataopen event to be handled
		 */
		virtual void handle_event(dataopen& ev)=0;
	};

	/** Construct dataopen event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	dataopen(desktop::component& target,int wimpcode,
		os::wimp_block& wimpblock);

	/** Destroy dataopen event.
	 */
	virtual ~dataopen();
protected:
	virtual bool deliver(desktop::component& dest);
public:
	/** Get destination window handle.
	 * @return the destination window handle
	 */
	int whandle() const;

	/** Get destination position.
	 * @return the destination position
	 */
	point position() const;

	/** Get filetype of data.
	 * @return the filetype of the data
	 */
	unsigned int filetype() const;

	/** Get proposed leafname for data.
	 * @return the proposed leafname
	 */
	string pathname() const;

	/** Send Message_DataLoadAck in reply to this event.  */
	void reply() const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
