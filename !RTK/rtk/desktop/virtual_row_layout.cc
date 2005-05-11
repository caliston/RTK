// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/virtual_row_layout.h"

namespace rtk {
namespace desktop {

using rtk::util::divider;

virtual_row_layout::virtual_row_layout(size_type xcells):
	_xcells(xcells),
	_xmin(xcells+1,0),
	_xgap(0)
{}

virtual_row_layout::~virtual_row_layout()
{
	remove();
}

box virtual_row_layout::auto_bbox() const
{
	// Reset y-baseline set.
	_ybs=ybaseline_set();

	// Request min_bbox for each cell.  Incorporate into y-baseline set
	// and total width.
	int xsize=0;
	for (size_type x=0;x!=_xcells;++x)
	{
		box mcbbox=cell_min_bbox(x);
		_ybs.add(mcbbox,cell_ybaseline(x));
		xsize+=mcbbox.xsize();
	}

	// Calculate combined height.
	int ysize=_ybs.ysize();

	// Add gaps to total width.
	if (_xcells) xsize+=(_xcells-1)*_xgap;

	// Add margin to width and height.
	xsize+=_margin.xsize();
	ysize+=_margin.ysize();

	// Construct minimum bounding box, with respect to top left-hand
	// corner of layout.
	box abbox(0,-ysize,xsize,0);

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

box virtual_row_layout::bbox() const
{
	return _bbox;
}

void virtual_row_layout::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// any children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	// Remove margin.
	box ibox(_bbox-_margin);

	// Get bounding box before adding excess.
	box abbox=auto_bbox();

	// Calculate excess.
	int xexcess=_bbox.xsize()-abbox.xsize();
	if (xexcess<0) xexcess=0;
	int xspread=abbox.xsize();
	xspread-=_margin.xsize();
	if (_xcells) xspread-=(_xcells-1)*_xgap;
	if (xspread<0) xspread=0;
	divider xdiv(xexcess,xspread);

	// Set minimum x-coordinate for each cell.
	int xpos=ibox.xmin();
	_xmin[0]=xpos;
	for (size_type x=0;x!=_xcells;++x)
	{
		// Get minimum bounding box for cell.
		box mcbbox=cell_min_bbox(x);

		// Set minimum x-coordinate for next cell.
		int xsize=mcbbox.xsize();
		xpos+=xsize+xdiv(xsize);
		xpos+=_xgap;
		_xmin[x+1]=xpos;
	}

	// Place children.
	for (size_type x=0;x!=_xcells;++x)
	{
		// Create x-baseline set for just this cell.
		xbaseline_set xbs;
		xbs.add(cell_min_bbox(x),cell_xbaseline(x));

		// Construct bounding box for cell with respect to origin
		// of layout.
		box cbbox(_xmin[x],ibox.ymin(),_xmin[x+1]-_xgap,ibox.ymax());

		// Calculate offset from top left-hand corner of cell to
		// origin of child.
		int xoffset=xbs.offset(xbaseline_left,cell_xbaseline(x),
			cbbox.xsize());
		int yoffset=_ybs.offset(ybaseline_bottom,cell_ybaseline(x),
			cbbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(cbbox.xminymin()+coffset);

		// Reformat child.
		cell_reformat(x,cpos,cbbox-cpos);
	}
}

void virtual_row_layout::redraw(gcontext& context,const box& clip)
{
	// Remove margin.
	box ibox(_bbox-_margin);

	// Look for the first column with a right edge which overlaps (or is to
	// the right of) the clip box: _xmin[x0+1] -_xgap > clip.xmin().
	std::vector<int>::iterator xf0=upper_bound(
		_xmin.begin(),_xmin.end(),clip.xmin()+_xgap,std::less<int>());
	size_type x0=xf0-_xmin.begin();
	if (x0) --x0;

	// Look for the first column with a left edge which is to the right of
	// the clip box: _xmin[x1] >= clip.xmax().
	std::vector<int>::iterator xf1=lower_bound(
		_xmin.begin(),_xmin.end(),clip.xmax(),std::less<int>());
	size_type x1=xf1-_xmin.begin();
	if (x1>_xcells) x1=_xcells;

	// Redraw children.
	// For safety, use an inequality in the for-loop.
	for (size_type x=x0;x<x1;++x)
	{
		// Create x-baseline set for just this cell.
		xbaseline_set xbs;
		xbs.add(cell_min_bbox(x),cell_xbaseline(x));

		// Construct bounding box for cell with respect to origin
		// of layout.
		box cbbox(_xmin[x],ibox.ymin(),_xmin[x+1]-_xgap,ibox.ymax());

		// Calculate offset from top left-hand corner of cell to
		// origin of child.
		int xoffset=xbs.offset(xbaseline_left,cell_xbaseline(x),
			cbbox.xsize());
		int yoffset=_ybs.offset(ybaseline_bottom,cell_ybaseline(x),
			cbbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(cbbox.xminymin()+coffset);

		// Redraw cell.
		context+=cpos;
		cell_redraw(x,context,clip-cpos);
		context-=cpos;
	}
	inherited::redraw(context,clip);
}

virtual_row_layout& virtual_row_layout::cells(size_type xcells)
{
	_xcells=xcells;
	_xmin.resize(xcells+1,0);
	invalidate();
	return *this;
}

virtual_row_layout& virtual_row_layout::xgap(int xgap)
{
	_xgap=xgap;
	invalidate();
	return *this;
}

virtual_row_layout& virtual_row_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

virtual_row_layout& virtual_row_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

virtual_row_layout::xbaseline_type virtual_row_layout::cell_xbaseline(
	size_type x) const
{
	return xbaseline_text;
}

virtual_row_layout::ybaseline_type virtual_row_layout::cell_ybaseline(
	size_type x) const
{
	return ybaseline_text;
}

void virtual_row_layout::cell_reformat(size_type x,const point& origin,
	const box &bbox)
{}

virtual_row_layout::size_type virtual_row_layout::find_x(const point& p)
{
	// Look for the first cell with a left edge which is to the right
	// of the point: _xmin[x] > p.x().
	std::vector<int>::const_iterator xf=
		upper_bound(_xmin.begin(),_xmin.end(),p.x(),std::less<int>());
	if (xf==_xmin.begin()) return npos;
	size_type x=(xf-_xmin.begin())-1;

	// Return npos if point outside cell limits, otherwise return y.
	if ((p.x()<_xmin[x])||(p.x()>=_xmin[x+1]-_xgap)) return npos;
	return x;
}

}; /* namespace desktop */
}; /* namespace rtk */
