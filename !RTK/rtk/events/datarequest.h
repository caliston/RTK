// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_DATAREQUEST
#define _RTK_EVENTS_DATAREQUEST

#include <vector>
#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/message.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace events {

using rtk::graphics::point;

/** A class to represent a RISC OS Message_DataRequest user message. */
class datarequest:
	public message
{
public:
	/** A mixin class for handling datarequest events.
	 * If a class wishes to receive datarequest events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle datarequest event.
		 * @param ev the datarequest event to be handled
		 */
		virtual void handle_event(datarequest& ev)=0;
	};

	/** Construct datarequest event.
	 * @param target the target of the event
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	datarequest(desktop::component& target,int wimpcode,
		const os::wimp_block& wimpblock);

	/** Destroy datarequest event.
	 */
	virtual ~datarequest();
protected:
	virtual bool deliver(desktop::component& dest);
public:
	/** Get destination window handle.
	 * @return the destination window handle
	 */
	int whandle() const;

	/** Get icon handle.
	 * @return the icon handle
	 */
	int ihandle() const;

	/** Get destination position.
	 * @return the destination position
	 */
	point position() const;

	/** Get flags.
	 * @return the flags
	 */
	int flags() const;

	/** Test whether the clipboard has been requested.
	 * @return true if the clipboard has been requested, otherwise false
	 */
	bool request_clipboard() const;

	/** Get list of acceptable filetypes
	 * @return the list of acceptable filetypes, in order of preference
	 */
	std::vector<unsigned int> filetypes() const;

	/** Send Message_DataSave in reply to this event.
	 * @param filetype the filetype of the data
	 * @param estsize the estimated size of the data
	 */
	void reply(unsigned int filetype,unsigned int estsize) const;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
