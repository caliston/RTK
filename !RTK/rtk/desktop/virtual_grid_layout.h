// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_VIRTUAL_GRID_LAYOUT
#define _RTK_DESKTOP_VIRTUAL_GRID_LAYOUT

#include <vector>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging components in a grid.
 * This class produces the same visual appearance as a grid_layout,
 * without the need for the members of the row to exist as components.
 */
class virtual_grid_layout:
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
	/** The number of columns. */
	size_type _xcells;

	/** The number of rows. */
	size_type _ycells;

	/** A vector containing the position of the left edge of each cell
	 * with respect to the origin of the layout.  The position of the
	 * right edge is obtained by subtracting _xgap from the left edge of
	 * the following cell.  A hypothetical value for _xmin[xcells()] is
	 * included in the vector so that the position of the right edge
	 * of the last cell can be determined. */
	std::vector<int> _xmin;

	/** A vector containing the position of the top edge of each cell
	 * with respect to the origin of the layout.  The position of the
	 * bottom edge is obtained by adding _ygap from the top edge of
	 * the following cell.  A hypothetical value for _ymax[ycells()] is
	 * included in the vector so that the position of the bottom edge
	 * of the last cell can be determined. */
	std::vector<int> _ymax;

	/** A vector containing a cached x-baseline set for each column. */
	mutable std::vector<xbaseline_set> _xbs;

	/** A vector containing a cached y-baseline set for each row. */
	mutable std::vector<ybaseline_set> _ybs;

	/** The size of gap to be placed between columns. */
	int _xgap;

	/** The size of gap to be placed between rows. */
	int _ygap;

	/** The margin to be placed around the whole layout. */
	box _margin;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct virtual grid layout.
	 * @param xcells the required number of columns (defaults to 0)
	 * @param ycells the required number of rows (defaults to 0)
	 */
	virtual_grid_layout(size_type xcells=0,size_type ycells=0);

	/** Destroy virtual grid layout. */
	virtual ~virtual_grid_layout();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void redraw(gcontext& context,const box& clip);
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
	virtual_grid_layout& cells(size_type xcells,size_type ycells);

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
	virtual_grid_layout& xgap(int xgap);

	/** Set size of gap between rows.
	 * @param ygap the required gap between rows
	 * @return a reference to this
	 */
	virtual_grid_layout& ygap(int ygap);

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
	virtual_grid_layout& margin(const box& margin);

	/** Set margin around layout.
	 * @param margin an integer specifying the required margin width for
	 *  all sides of the layout
	 * @return a reference to this
	 */
	virtual_grid_layout& margin(int margin);

	/** Get minimum bounding box for cell.
	 * This function is equivalent to get_bbox() but acts on a virtual
	 * child instead of a real component.
	 * @param x the x-index of the cell
	 * @param y the y-index of the cell
	 */
	virtual box cell_min_bbox(size_type x,size_type y) const=0;

	/** Get vertical baseline for cell.
	 * This function is equivalent to xbaseline() but acts on a virtual
	 * child instead of a real component.  By default it returns
	 * xbaseline_text.
	 * @param x the x-index of the cell
	 * @param y the y-index of the cell
	 */
	virtual xbaseline_type cell_xbaseline(size_type x,size_type y) const;

	/** Get horizontal baseline for cell.
	 * This function is equivalent to ybaseline() but acts on a virtual
	 * child instead of a real component.  By default it returns
	 * ybaseline_text.
	 * @param x the x-index of the cell
	 * @param y the y-index of the cell
	 */
	virtual ybaseline_type cell_ybaseline(size_type x,size_type y) const;

	/** Reformat cell to fit within specified bounding box.
	 * This function is equivalent to reformat() but acts on a virtual
	 * child instead of a real component.  By default it does nothing.
	 * @param x the x-index of the cell
	 * @param y the y-index of the cell
	 * @param origin the new origin of this cell, with respect to
	 *  its parent
	 * @param bbox the available bounding box for this cell, with
	 *  respect to its own origin
	 */
	virtual void cell_reformat(size_type x,size_type y,const point& origin,
		const box &bbox);

	/** Redraw call.
	 * This function is equivalent to redraw() but acts on a virtual
	 * child instead of a real component.
	 * @param x the x-index of the cell
	 * @param y the y-index of the cell
	 * @param context the graphics context within which the
	 *  redraw should take place
	 * @param clip the bounding box of the region to be redrawn,
	 *  with respect to the origin of the cell
	 */
	virtual void cell_redraw(size_type x,size_type y,gcontext& context,
		const box& clip)=0;

	/** Get x-index of cell containing point.
	 * @param p the point to find, with respect to the origin of this
	 *  component.
	 * @return the x-index of the cell containing p, or npos if there is
	 *  no such cell.
	 */
	size_type find_x(const point& p);

	/** Get y-index of cell containing point.
	 * @param p the point to find, with respect to the origin of this
	 *  component.
	 * @return the y-index of the cell containing p, or npos if there is
	 *  no such cell.
	 */
	size_type find_y(const point& p);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
