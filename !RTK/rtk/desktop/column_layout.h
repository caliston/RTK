// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_COLUMN_LAYOUT
#define _RTK_DESKTOP_COLUMN_LAYOUT

#include <vector>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging components in a column.
 * It is possible to specify the gap to be placed between cells and the
 * margin to be placed around the layout as a whole.  Cells that share
 * the same vertical baseline are aligned with each other. The layout
 * will expand to fill any extra space made available to it. Note that
 * the algorithm used to do this is implementation-dependent, so may
 * change in future versions of this library.
 */
class column_layout:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;
public:
	/** A type for representing cell counts or indices. */
	typedef unsigned int size_type;

	/** A null value for use in place of a cell index. */
	static const size_type npos=static_cast<size_type>(-1);
private:
	/** Vector containing child pointers for each cell. */
	std::vector<component*> _components;

	/** Vector containing position of top edge of each cell with
	 * respect to origin of layout.  The position of the bottom edge
	 * is obtained by adding _ygap from the top edge of the
	 * following cell.  A hypothetical value for _ymax[ycells()] is
	 * included in the vector so that the position of the bottom edge
	 * of the last cell can be determined. */
	std::vector<int> _ymax;

	/** The cached x-baseline set for the layout. */
	mutable xbaseline_set _xbs;

	/** The size of gap between cells. */
	int _ygap;

	/** The margin to be placed around the whole layout. */
	box _margin;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct column layout.
	 * @param ycells the required number of cells (defaults to 0)
	 */
	column_layout(size_type ycells=0);

	/** Destroy column layout. */
	virtual ~column_layout();

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
	/** Get number of cells.
	 * @return the number of cells
	 */
	size_type ycells() const
		{ return _components.size(); }

	/** Set number of cells.
	 * @param ycells the required number of cells
	 * @return a reference to this
	 */
	column_layout& cells(size_type ycells);

	/** Add component to layout.
	 * If a component already exists at the specified position
	 * then it is replaced.
	 * @param c the component to be added
	 * @param y the cell index (defaults to npos, meaning new cell)
	 * @return a reference to this
	 */
	column_layout& add(component& c,size_type y=npos);

	/** Get size of gap between cells.
	 * @return the size of gap between cells
	 */
	int ygap() const
		{ return _ygap; }

	/** Set size of gap between cells.
	 * @param ygap the required gap between cells
	 * @return a reference to this
	 */
	column_layout& ygap(int ygap);

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
	column_layout& margin(const box& margin);

	/** Set margin around layout.
	 * @param margin an integer specifying the required margin width for
	 *  all sides of the layout
	 * @return a reference to this
	 */
	column_layout& margin(int margin);
};

} /* namespace desktop */
} /* namespace rtk */

#endif
