// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_DRAGASPRITE
#define _RTK_OS_DRAGASPRITE

#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

using rtk::graphics::box;

struct sprite_area;

/** Start dragging sprite.
 * The flags are:
 * - bits 0-1 = horizontal location (00=left, 01=centre, 10=right)
 * - bits 2-3 = vertical location (00=bottom, 01=centre, 10=top)
 * - bits 4-5 = parent bounding box identity (00=screen, 01=window, 10=pbox)
 * - bit 6 = parent bounding box applies to (0=dragged box, 1=pointer)
 * - bit 7 = drop shadow (0=false, 1=true)
 *
 * @param flags the flags
 * @param area the sprite area (0=system, 1=wimp)
 * @param name the sprite name
 * @param dbox the dragged box
 * @param pbox the parent box
 */
void DragASprite_Start(int flags,sprite_area* area,const char* name,
	box* dbox,box* pbox);

/** Stop dragging sprite. */
void DragASprite_Stop();

}; /* namespace os */
}; /* namespace rtk */

#endif
