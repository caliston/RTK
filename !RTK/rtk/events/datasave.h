// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_DATASAVE
#define _RTK_EVENTS_DATASAVE

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

using std::string;
using rtk::graphics::point;

/** A class to represent a RISC OS Message_DataSave user message. */
class datasave:
	public message
{
public:
	/** A mixin class for handling datasave events.
	 * If a class wishes to receive datasave events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle datasave event.
		 * @param ev the datasave event to be handled
		 */
		virtual void handle_event(datasave& ev)=0;
	};

	/** Construct datasave event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	datasave(desktop::component& target,int wimpcode,
		const os::wimp_block& wimpblock);

	/** Destroy datasave event.
	 */
	virtual ~datasave();
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

	/** Get proposed leafname for data.
	 * @return the proposed leafname
	 */
	string leafname() const;

	/** Send Message_RAMFetch in reply to this event.
	 * @param buffer the data transfer buffer
	 * @param buffer_size the size of the data transfer buffer
	 */
	void reply(void* buffer,unsigned int buffer_size) const;

	/** Send Message_DataSaveAck in reply to this event.
	 * The pathname is secure if it is permanent location for the file
	 * to be stored (as opposed to a temporary name such as <Wimp$Scrap>).
	 * @param pathname the required pathname
	 * @param secure true if pathname is secure, otherwise false
	 */
	void reply(const string& pathname,bool secure=false) const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
