// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_VIRTUAL_ROW_LAYOUT
#define _RTK_DESKTOP_VIRTUAL_ROW_LAYOUT

#include <vector>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging cells in a row.
 * This class produces the same visual appearance as a row_layout,
 * without the need for the members of the row to exist as components.
 */
class virtual_row_layout:
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
	/** The number of cells. */
	size_type _xcells;

	/** A vector containing the position of the left edge of each cell
	 * with respect to origin of layout.  The position of the right
	 * edge is obtained by subtracting _xgap from the left edge of the
	 * following cell.  A hypothetical value for _xmin[xcells()] is
	 * included in the vector so that the position of the right edge
	 * of the last cell can be determined. */
	std::vector<int> _xmin;

	/** The cached y-baseline set for the layout. */
	mutable ybaseline_set _ybs;

	/** The size of gap to be placed between cells. */
	int _xgap;

	/** The margin to be placed around the whole layout. */
	box _margin;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct virtual row layout.
	 * @param xcells the required number of cells (defaults to 0)
	 */
	virtual_row_layout(size_type xcells=0);

	/** Destroy virtual row layout. */
	virtual ~virtual_row_layout();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void redraw(gcontext& context,const box& clip);
public:
	/** Get number of cells.
	 * @return the number of cells
	 */
	size_type xcells() const
		{ return _xcells; }

	/** Set number of cells.
	 * @param xcells the required number of cells
	 * @return a reference to this
	 */
	virtual_row_layout& cells(size_type xcells);

	/** Get size of gap between cells.
	 * @return the size of gap between cells
	 */
	int xgap() const
		{ return _xgap; }

	/** Set size of gap between cells.
	 * @param xgap the required gap between cells
	 * @return a reference to this
	 */
	virtual_row_layout& xgap(int xgap);

	/** Get margin around layout.
	 * @return a box indicating the margin width for each side of the layout
	 */
	box margin() const
		{ return _margin; }

	/** Set margin around layout.
	 * @param margin a box specifying the required margin width for each
	 *  side of the layout
	 * @return a reference to this
	 */
	virtual_row_layout& margin(const box& margin);

	/** Set margin around layout.
	 * @param margin an integer specifying the required margin width for
	 *  all sides of the layout
	 * @return a reference to this
	 */
	virtual_row_layout& margin(int margin);

	/** Get minimum bounding box for cell.
	 * This function is equivalent to get_bbox() but acts on a virtual
	 * child instead of a real component.
	 * @param x the x-index of the cell
	 */
	virtual box cell_min_bbox(size_type x) const=0;

	/** Get vertical baseline for cell.
	 * This function is equivalent to xbaseline() but acts on a virtual
	 * child instead of a real component.  By default it returns
	 * xbaseline_text.
	 * @param x the x-index of the cell
	 */
	virtual xbaseline_type cell_xbaseline(size_type x) const;

	/** Get horizontal baseline for cell.
	 * This function is equivalent to ybaseline() but acts on a virtual
	 * child instead of a real component.  By default it returns
	 * ybaseline_text.
	 * @param x the x-index of the cell
	 */
	virtual ybaseline_type cell_ybaseline(size_type x) const;

	/** Reformat cell to fit within specified bounding box.
	 * This function is equivalent to reformat() but acts on a virtual
	 * child instead of a real component.  By default it does nothing.
	 * @param x the x-index of the cell
	 * @param origin the new origin of this cell, with respect to
	 *  its parent
	 * @param bbox the available bounding box for this cell, with
	 *  respect to its own origin
	 */
	virtual void cell_reformat(size_type x,const point& origin,
		const box &bbox);

	/** Redraw call.
	 * This function is equivalent to redraw() but acts on a virtual
	 * child instead of a real component.
	 * @param x the x-index of the cell
	 * @param context the graphics context within which the
	 *  redraw should take place
	 * @param clip the bounding box of the region to be redrawn,
	 *  with respect to the origin of the cell
	 */
	virtual void cell_redraw(size_type x,gcontext& context,const box& clip)=0;

	/** Get x-index of cell containing point.
	 * @param p the point to find, with respect to the origin of this
	 *  component.
	 * @return the x-index of the cell containing p, or npos if there is
	 *  no such cell.
	 */
	size_type find_x(const point& p);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
