// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_ABSOLUTE_LAYOUT
#define _RTK_DESKTOP_ABSOLUTE_LAYOUT

#include <vector>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging components by specifying their coordinates.
 * This class is provided for use when the required layout does not have a
 * logical structure, or where the structure is too complex to be represented
 * using other layout classes (such as rows, columns and grids).
 */
class absolute_layout:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** A vector containing a pointer to each child. */
	std::vector<component*> _components;

	/** The margin to be placed around the whole layout. */
	box _margin;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct absolute layout.
	 * By default, the origin of an absolute layout is in the top
	 * left-hand corner.
	 */
	absolute_layout();

	/** Destroy absolute layout. */
	virtual ~absolute_layout();

	virtual box auto_bbox() const;
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
protected:
	virtual void remove_notify(component& c);
public:
	/** Add component to layout.
	 * @param c the component to be added
	 * @param p the required initial origin
	 * @return a reference to this
	 */
	absolute_layout& add(component& c,const point& p);

	/** Get margin around layout.
	 * @return a box indicating the margin width for each side of the layout
	 */
	const box& margin() const
		{ return _margin; }

	/** Set margin around layout.
	 * @param margin a box specifying the required margin width for each
	 *  side of the layout
	 * @return a reference to this
	 */
	absolute_layout& margin(const box& margin);

	/** Set margin around layout.
	 * @param margin an integer specifying the required margin width for
	 *  all sides of the layout
	 * @return a reference to this
	 */
	absolute_layout& margin(int margin);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
