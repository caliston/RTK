// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_EVENTS_REDIRECTION
#define _RTK_EVENTS_REDIRECTION

namespace rtk {
namespace desktop {

class component;

}; /* namespace desktop */

namespace events {

/** A class for redirecting events.
 * This is a mixin class which allow a component to redirect event delivery.
 */
class redirection
{
private:
	/** The redirected parent, or 0 if redirection is disabled. */
	desktop::component* _redirect;
public:
	/** Create redirection mixin.
	 * By default, redirection is disabled.
	 */
	redirection();

	/** Destroy redirection mixin.
	 * Note that this class must have at least one virtual method
	 * so that dynamic_cast<>() will work correctly.
	 */
	virtual ~redirection();

	/** Get redirected parent.
	 * @return the redirected parent, or 0 if redirection is disabled.
	 */
	desktop::component* redirect() const
		{ return _redirect; }

	/** Set redirected parent.
	 * @param redirect the redirected parent, or 0 to disable redirection.
	 */
	void redirect(desktop::component* redirect);
};

}; /* namespace events */
}; /* namespace rtk */

#endif
