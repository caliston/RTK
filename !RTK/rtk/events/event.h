// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_EVENT
#define _RTK_EVENTS_EVENT

namespace rtk {
namespace graphics {};

namespace desktop {

class component;

}; /* namespace desktop */

namespace events {

/** An abstract class to represent an RTK event.
 */
class event
{
private:
	/** The target of the event. */
	desktop::component* _target;
public:
	/** Construct event.
	 * @param target the target of the event
	 */
	event(desktop::component& target);

	/** Destroy event.
	 */
	virtual ~event();

	/** Get target.
	 * @return the target of the event
	 */
	desktop::component* target() const
		{ return _target; }

	/** Post event.
	 * Search upwards from the target for a component with a suitable
	 * handler for this event, then pass this event to that handler.
	 * Continue until all suitable handlers have been called.
	 * @return true if one or more suitable handlers were found,
	 *  otherwise false
	 */
	bool post();
protected:
	/** Deliver event to component.
	 * If the specified destination has a handler method for this event
	 * then call that method.
	 * @param dest the destination to which this event should be delivered
	 * @return true if a handler was found, otherwise false
	 */
	virtual bool deliver(desktop::component& dest)=0;
};

}; /* namespace events */
}; /* namespace rtk */

#endif
