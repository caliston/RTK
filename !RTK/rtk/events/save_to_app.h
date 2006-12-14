// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_SAVE_TO_APP
#define _RTK_EVENTS_SAVE_TO_APP

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/events/event.h"

namespace rtk {
namespace events {

using std::string;
using rtk::graphics::point;

/** An event class for requesting that data be saved to a given application. */
class save_to_app:
	public event
{
private:
	/** The destination window handle. */
	int _whandle;

	/** The destination icon handle. */
	int _ihandle;

	/** The destination position. */
	point _position;

	/** The leafname to which the data should be saved. */
	string _leafname;
public:
	/** A mixin class for handling save_to_app events.
	 * If a class wishes to receive save_to_app events then it should
	 * inherit from this mixin class and provide an implementation for
	 * handle_event().
	 */
	class handler
	{
	public:
		/** Handle save_to_app event.
		 * @param ev the save_to_app event to be handled
		 */
		virtual void handle_event(save_to_app& ev)=0;
	};

	/** Construct save_to_app event.
	 * The destination window handle, icon handle and position are obtained
	 * using Wimp_GetPointerInfo.
	 * @param target the target of the event
	 * @param leafname the leafname to which the data should be saved
	 */
	save_to_app(desktop::component& target,const string& leafname);

	/** Construct save_to_app event.
	 * @param target the target of the event
	 * @param whandle the destination window handle
	 * @param ihandle the destination icon handle
	 * @param position the destination position
	 * @param leafname the leafname to which the data should be saved
	 */
	save_to_app(desktop::component& target,int whandle,int ihandle,
		const point& position,const string& leafname);

	/** Destroy save_to_app event.
	 */
	virtual ~save_to_app();

	/** Get destination window handle.
	 * @return the destination window handle
	 */
	int whandle() const
		{ return _whandle; }

	/** Get destination icon handle.
	 * @return the destination icon handle
	 */
	int ihandle() const
		{ return _ihandle; }

	/** Get destination position.
	 * @return the destination position
	 */
	point position() const
		{ return _position; }

	/** Get leafname.
	 * @return the leafname to which the data should be saved
	 */
	const string& leafname() const
		{ return _leafname; }
protected:
	virtual bool deliver(desktop::component& dest);
};

} /* namespace events */
} /* namespace rtk */

#endif
