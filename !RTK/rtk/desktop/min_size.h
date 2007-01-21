// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_MIN_SIZE
#define _RTK_DESKTOP_MIN_SIZE

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A class to impose a minimum size on its child component.
 * This component can contain one child component, which should expand to
 * the size of the minimum box if it is smaller. This class will ensure
 * that the minimum size is always available, but will expand to fit the
 * child if the child is larger than the minimum size.
 */
class min_size:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** The minimum size. */
	point _min_size;

	/** The child component. */
	component *_content;

public:
	/** Construct min_size. */
	min_size();

	/** Destroy min_size. */
	virtual ~min_size();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);

	/** Add the child component.
	 * @param c the child component to add
	 * @return a reference to this
	 */
	min_size& add(component& c);

	/** Get the minimum size.
	 * @return the minimum bounding box
	 */
	point minimum_size() const { return _min_size; }

	/** Set the minimum size box.
	 * @param size the minimum bounding box
	 * @return a reference to this
	 */
	min_size& minimum_size(const point& size);

};

} /* namespace desktop */
} /* namespace rtk */

#endif
