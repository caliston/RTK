// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SIZEABLE_COMPONENT
#define _RTK_SIZEABLE_COMPONENT

#include "rtk/desktop/component.h"

namespace rtk {
namespace desktop {

/** An abstract class from which RTK components may be derived if it is
 * possible to explicitly specify the required minimum bounding box.
 */
class sizeable_component:
	public component
{
private:
	/** The class from which this one is derived. */
	typedef component inherited;
public:
	using component::xauto;
	using component::yauto;
private:
	using component::set_xauto;
	using component::set_yauto;
	/** The minimum bounding box. */
	mutable box _min_bbox;
public:
	/** Construct sizeable component.
	 * @param origin the initial origin of the component, with respect
	 *  to its parent
	 */
	sizeable_component(const point& origin=point());

	/** Destroy sizeable component. */
	virtual ~sizeable_component();

	virtual box min_bbox() const;
	virtual void resize() const;

	/** Set minimum bounding box.
	 * If the specified width is zero then the width of the minimum bounding
	 * box will be varied automatically to fit the content of the component.
	 * Similarly for the height.
	 * @param min_bbox the required minimum bounding box
	 */
	sizeable_component& min_bbox(const box& min_bbox);

	/** Set size of minimum bounding box.
	 * The position of the bounding box is chosen to suit the baselines.
	 */
	sizeable_component& min_size(const point& min_size);

	/** Get bounding box needed to fit content.
	 * @return the bounding box needed to fit the content of this component
	 */
	virtual box auto_bbox() const=0;
};

} /* namespace desktop */
} /* namespace rtk */

#endif
