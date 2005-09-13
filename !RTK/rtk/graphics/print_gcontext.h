// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_PRINT_GCONTEXT
#define _RTK_GRAPHICS_PRINT_GCONTEXT

#include "rtk/graphics/vdu_gcontext.h"

namespace rtk {
namespace graphics {

/** A class to represent a graphics context associated with a print job.
 * Each graphics context of this type has an associated file handle.
 * While the context is active, output is redirected to that file
 * handle (and is presumed to remain so until the context is deactivated).
 *
 * The file handle is passed into the constructor and it must remain open
 * for at least the lifetime of the graphics context.  It is for the user
 * of this class to open and close the file handle.
 */
class print_gcontext:
	public vdu_gcontext
{
private:
	/** The file handle to which output is redirected. */
	int _handle;
public:
	/** Construct printer graphics context.
	 * @param origin the initial origin
	 * @param handle the file handle to which output is to be redirected
	 */
	print_gcontext(const point& origin,int handle);

	/** Destroy printer graphics context. */
	virtual ~print_gcontext();
protected:
	virtual void activate();
	virtual void deactivate();
};

}; /* namespace graphics */
}; /* namespace rtk */

#endif
