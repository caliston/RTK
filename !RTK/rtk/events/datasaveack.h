// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_DATASAVEACK
#define _RTK_EVENTS_DATASAVEACK

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Message_DataSaveAck user message. */
class datasaveack:
	public message
{
public:
	/** A mixin class for handling datasaveack events.
	 * If a class wishes to receive datasaveack events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle datasaveack event.
		 * @param ev the datasaveack event to be handled
		 */
		virtual void handle_event(datasaveack& ev)=0;
	};

	/** Construct datasaveack event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	datasaveack(desktop::component& target,int wimpcode,
		const os::wimp_block& wimpblock);

	/** Destroy datasaveack event.
	 */
	virtual ~datasaveack();
protected:
	virtual bool deliver(desktop::component& dest);
public:
	/** Get destination window handle.
	 * @return the destination window handle
	 */
	int whandle() const;

	/** Get destination icon handle.
	 * @return the destination icon handle
	 */
	int ihandle() const;

	/** Get destination position.
	 * @return the destination position
	 */
	point position() const;

	/** Get estimated size of data.
	 * @return the estimated size of the data
	 */
	unsigned int estsize() const;

	/** Get filetype of data.
	 * @return the filetype of the data
	 */
	unsigned int filetype() const;

	/** Get pathname for data.
	 * @return the pathname
	 */
	string pathname() const;

	/** Send Message_DataLoad in reply to this event. */
	void reply() const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
