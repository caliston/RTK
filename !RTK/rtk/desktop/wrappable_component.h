// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_WRAPPABLE_COMPONENT
#define _RTK_WRAPPABLE_COMPONENT

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** An abstract class from which a wrappable RTK component may be derived.
 * An implementation of this class should provide definitions for both
 * auto_bbox and auto_wrap_bbox.  These indicate the bounding box needed
 * to fit the content of the component, either without or with line
 * wrapping respectively.
 */
class wrappable_component:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;
private:
	/** The line-wrapped minimum bounding box.
	 * With line wrapping there are many possible minimum bounding boxes.
	 * This member is used to cache one of those possibilities in order
	 * to reduce the number of times that auto_wrap_bbox is called.
	 */
	mutable box _min_wrap_bbox;
public:
	/** Construct wrappable component.
	 * @param origin the initial origin of the component, with respect
	 *  to its parent
	 */
	wrappable_component(const point& origin=point());

	/** Destroy wrappable component. */
	virtual ~wrappable_component();

	virtual box min_wrap_bbox(const box& wbox) const;
	virtual void resize() const;

	/** Get line-wrapped bounding box needed to fit content.
	 * @return the line-wrapped bounding box needed to fit the content
	 *  of this component
	 */
	virtual box auto_wrap_bbox(const box& wbox) const=0;
};

} /* namespace desktop */
} /* namespace rtk */

#endif
