// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_GRID_LAYOUT
#define _RTK_DESKTOP_GRID_LAYOUT

#include <vector>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging components in a grid.
 * It is possible to specify the horizontal or vertical gap to be placed
 * between cells and the margin to be placed around the layout as a whole.
 * Cells that share the same horizontal or vertical baseline are aligned
 * with each other. The layout will expand to fill any extra space made
 * available to it. Note that the algorithm used to do this is
 * implementation-dependent, so may change in future versions of this
 * library.
 */
class grid_layout:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;
public:
	/** A type for representing cell counts or indices. */
	typedef unsigned int size_type;
private:
	/** The number of columns. */
	size_type _xcells;

	/** The number of rows. */
	size_type _ycells;

	/** Vector containing child pointer for each cell. */
	vector<component*> _components;

	/** Vector containing position of left edge of each cell with
	 * respect to origin of layout.  The position of the right edge
	 * is obtained by subtracting _xgap from the left edge of the
	 * following cell.  A hypothetical value for _xmin[xcells()] is
	 * included in the vector so that the position of the right edge
	 * of the last cell can be determined. */
	vector<int> _xmin;

	/** Vector containing position of top edge of each cell with
	 * respect to origin of layout.  The position of the bottom edge
	 * is obtained by adding _ygap from the top edge of the
	 * following cell.  A hypothetical value for _ymax[ycells()] is
	 * included in the vector so that the position of the bottom edge
	 * of the last cell can be determined. */
	vector<int> _ymax;

	/** Vector containing cached x-baseline set for each cell. */
	mutable vector<xbaseline_set> _xbs;

	/** Vector containing cached y-baseline set for each cell. */
	mutable vector<ybaseline_set> _ybs;

	/** The size of gap to be placed between columns. */
	int _xgap;

	/** The size of gap to be placed between rows. */
	int _ygap;

	/** The margin to be placed around the whole layout. */
	box _margin;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct grid layout.
	 * @param xcells the required number of columns (defaults to 0)
	 * @param ycells the required number of rows (defaults to 0)
	 */
	grid_layout(size_type xcells=0,size_type ycells=0);

	/** Destroy grid layout. */
	virtual ~grid_layout();

	virtual box auto_bbox() const;
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& bbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
protected:
	virtual void remove_notify(component& c);
public:
	/** Get number of columns.
	 * @return the number of columns
	 */
	size_type xcells() const
		{ return _xcells; }

	/** Get number of rows.
	 * @return the number of rows
	 */
	size_type ycells() const
		{ return _ycells; }

	/** Set number of columns and rows.
	 * @param xcells the required number columns
	 * @param ycells the required number rows
	 * @return a reference to this
	 */
	grid_layout& cells(size_type xcells,size_type ycells);

	/** Add component to layout.
	 * If a component already exists at the specified position
	 * then it is replaced.
	 * @param c the component to be added
	 * @param x the column index
	 * @param y the row index
	 * @return a reference to this
	 */
	grid_layout& add(component& c,size_type x,size_type y);

	/** Get size of gap between columns.
	 * @return the size of gap between columns
	 */
	int xgap() const
		{ return _xgap; }

	/** Get size of gap between rows.
	 * @return the size of gap between rows
	 */
	int ygap() const
		{ return _ygap; }

	/** Set size of gap between columns.
	 * @param xgap the required gap between columns
	 * @return a reference to this
	 */
	grid_layout& xgap(int xgap);

	/** Set size of gap between rows.
	 * @param ygap the required gap between rows
	 * @return a reference to this
	 */
	grid_layout& ygap(int ygap);

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
	grid_layout& margin(const box& margin);

	/** Set margin around layout.
	 * @param margin an integer specifying the required margin width for
	 *  all sides of the layout
	 * @return a reference to this
	 */
	grid_layout& margin(int margin);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
