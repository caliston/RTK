// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_VDU_GCONTEXT
#define _RTK_GRAPHICS_VDU_GCONTEXT

#include "rtk/graphics/gcontext.h"

namespace rtk {
namespace graphics {

/** A class to represent a graphics context linked to the RISC OS VDU drivers.
 * For efficiency, this class assumes that it has exclusive control over
 * the RISC OS VDU drivers unless told otherwise.  If there is a posiblility
 * that the driver state has been changed by another agent (for example,
 * after a call to Wimp_Poll), then vdu_context::current() must be called
 * to ensure that the required state is re-established.
 */
class vdu_gcontext:
	public gcontext
{
private:
	/** The current VDU graphics context. */
	static vdu_gcontext* _current;
public:
	/** Get current VDU graphics context.
	 * @return the current context
	 */
	static vdu_gcontext* current();

	/** Set current VDU graphics context.
	 * If the current context is unchanged then no action is taken.
	 * If it is changed then any assumed state (currently the foreground
	 * and background colours) is re-established.
	 * @param current the required current context
	 */
	static void current(vdu_gcontext* current);
public:
	/** Construct VDU graphics context.
	 * @param origin the initial origin
	 * @param update true if this graphics context refers to an existing
	 *  valid area to be updated, false if it must be completely redrawn
	 */
	vdu_gcontext(const point& origin,bool update=false);

	/** Destroy VDU graphics context.
	 */
	virtual ~vdu_gcontext();

	virtual void plot(int code,const point& p);
	virtual void draw(const char* s,const point& p);
	virtual void draw(const font& f,const char* s,const point& p);

protected:
	virtual void fcolour_notify(int fcolour);
	virtual void bcolour_notify(int bcolour);

	/** Notify activation of graphics context.
	 * This function is called by vdu_gcontext::current to notify a
	 * graphics context that it has just become the current graphics
	 * context.
	 */
	virtual void activate();

	/** Notify deactivation of graphics context.
	 * This function is called by vdu_gcontext::current to notify a
	 * graphics context that it will shortly cease to be the current
	 * graphics context.
	 */
	virtual void deactivate();
};

}; /* namespace graphics */
}; /* namespace rtk */

#endif
