// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/row_layout.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;
using rtk::util::divider;

row_layout::row_layout(size_type xcells):
	basic_row_layout(xcells)
{}

box row_layout::auto_bbox() const
{
	// Determine number of cells.
	size_type xcells=_components.size();

	// Reset y-baseline set.
	_ybs=ybaseline_set();

	// Request min_bbox for each cell.  Incorporate into y-baseline set
	// and total width.
	int xsize=0;
	std::vector<component*>::const_iterator i=_components.begin();
	for (size_type x=0;x!=xcells;++x)
	{
		if (component* c=*i++)
		{
			box mcbbox=c->min_bbox();
			_ybs.add(mcbbox,c->ybaseline());
			xsize+=mcbbox.xsize();
		}
	}

	// Calculate combined height.
	int ysize=_ybs.ysize();

	// Add gaps to total width.
	if (xcells) xsize+=(xcells-1)*_xgap;

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

void row_layout::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Determine number of cells.
	size_type xcells=_components.size();

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

	box abbox=auto_bbox();

	// Calculate excess.
	int xexcess=_bbox.xsize()-abbox.xsize();
	if (xexcess<0) xexcess=0;
	int xspread=abbox.xsize();
	xspread-=_margin.xsize();
	if (xcells) xspread-=(xcells-1)*_xgap;
	if (xspread<0) xspread=0;
	divider xdiv(xexcess,xspread);

	// Set minimum x-coordinate for each cell.
	int xpos=ibox.xmin();
	_xmin[0]=xpos;
	for (size_type x=0;x!=xcells;++x)
	{
		// Get minimum bounding box for cell, if it is occupied.
		component* c=_components[x];
		box mcbbox=(c)?c->min_bbox():box();

		// Set minimum x-coordinate for next cell.
		int xsize=mcbbox.xsize();
		xpos+=xsize+xdiv(xsize);
		xpos+=_xgap;
		_xmin[x+1]=xpos;
	}

	// Place children.
	for (size_type x=0;x!=xcells;++x)
	{
		if (component* c=_components[x])
		{
			// Create x-baseline set for just this cell.
			xbaseline_set xbs;
			xbs.add(c->min_bbox(),c->xbaseline());

			// Construct bounding box for cell with respect to origin
			// of layout.
			box cbbox(_xmin[x],ibox.ymin(),_xmin[x+1]-_xgap,ibox.ymax());

			// Calculate offset from top left-hand corner of cell to
			// origin of child.
			int xoffset=xbs.offset(xbaseline_left,
				c->xbaseline(),cbbox.xsize());
			int yoffset=_ybs.offset(ybaseline_bottom,
				c->ybaseline(),cbbox.ysize());
			point coffset(xoffset,yoffset);

			// Calculate origin of cell with respect to origin of layout.
			point cpos(cbbox.xminymin()+coffset);

			// Reformat child.
			c->reformat(cpos,cbbox-cpos);
		}
	}
}

} /* namespace desktop */
} /* namespace rtk */
