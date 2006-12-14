// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_SAVE_TO_FILE
#define _RTK_EVENTS_SAVE_TO_FILE

#include <string>

#include "rtk/events/event.h"

namespace rtk {
namespace events {

using std::string;

/** An event class for requesting that data be saved to a given pathname. */
class save_to_file:
	public event
{
private:
	/** The pathname to which the data should be saved. */
	string _pathname;

	/** The selection flag.
	 * True if the save operation is restricted to the current selection,
	 * otherwise false.
	 */
	bool _selection;
public:
	/** A mixin class for handling save_to_file events.
	 * If a class wishes to receive save_to_file events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle save_to_file event.
		 * @param ev the save_to_file event to be handled
		 */
		virtual void handle_event(save_to_file& ev)=0;
	};

	/** Construct save_to_file event.
	 * @param target the target of the event
	 * @param pathname the pathname to which the data should be saved
	 * @param selection true if the save operation is restricted to the
	 *  current selection, otherwise false
	 */
	save_to_file(desktop::component& target,const string& pathname,
		bool selection);

	/** Destroy save_to_file event.
	 */
	virtual ~save_to_file();

	/** Get pathname.
	 * @return the pathname to which the data should be saved.
	 */
	const string& pathname()
		{ return _pathname; }

	/** Get selection flag.
	 * @return true if the save operation is restricted to the current
	 *  selection, otherwise false
	 */
	bool selection()
		{ return _selection; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
