// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/virtual_column_layout.h"

namespace rtk {
namespace desktop {

using rtk::util::divider;

virtual_column_layout::virtual_column_layout(size_type ycells):
	_ycells(ycells),
	_ymax(ycells+1,0),
	_ygap(0)
{}

virtual_column_layout::~virtual_column_layout()
{}

box virtual_column_layout::auto_bbox() const
{
	// Reset x-baseline set.
	_xbs=xbaseline_set();

	// Request min_bbox for each cell.  Incorporate into x-baseline set
	// and total height.
	int ysize=0;
	for (size_type y=0;y!=_ycells;++y)
	{
		box mcbbox=cell_min_bbox(y);
		_xbs.add(mcbbox,cell_xbaseline(y));
		ysize+=mcbbox.ysize();
	}

	// Calculate combined width.
	int xsize=_xbs.xsize();

	// Add gaps to total height.
	if (_ycells) ysize+=(_ycells-1)*_ygap;

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

box virtual_column_layout::bbox() const
{
	return _bbox;
}

void virtual_column_layout::reformat(const point& origin,const box &bbox)
{
	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// any children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw();
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw();

	// Remove margin.
	box ibox(_bbox-_margin);

	// Get bounding box before adding excess.
	box abbox=auto_bbox();

	// Calculate excess.
	int yexcess=_bbox.ysize()-abbox.ysize();
	if (yexcess<0) yexcess=0;
	int yspread=abbox.ysize();
	yspread-=_margin.ysize();
	if (_ycells) yspread-=(_ycells-1)*_ygap;
	if (yspread<0) yspread=0;
	divider ydiv(yexcess,yspread);

	// Set maximum y-coordinate for each cell.
	int ypos=ibox.ymax();
	_ymax[0]=ypos;
	for (size_type y=0;y!=_ycells;++y)
	{
		// Get minimum bounding box for cell.
		box mcbbox=cell_min_bbox(y);

		// Set maximum y-coordinate for next cell.
		int ysize=mcbbox.ysize();
		ypos-=ysize+ydiv(ysize);
		ypos-=_ygap;
		_ymax[y+1]=ypos;
	}

	// Place children.
	for (size_type y=0;y!=_ycells;++y)
	{
		// Create y-baseline set for just this cell.
		ybaseline_set ybs;
		ybs.add(cell_min_bbox(y),cell_ybaseline(y));

		// Construct bounding box for cell with respect to origin
		// of layout.
		box cbbox(ibox.xmin(),_ymax[y+1]+_ygap,ibox.xmax(),_ymax[y]);

		// Calculate offset from top left-hand corner of cell to
		// origin of child.
		int xoffset=_xbs.offset(xbaseline_left,cell_xbaseline(y),
			cbbox.xsize());
		int yoffset=ybs.offset(ybaseline_bottom,cell_ybaseline(y),
			cbbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(cbbox.xminymin()+coffset);

		// Reformat child.
		cell_reformat(y,cpos,cbbox-cpos);
	}
}

void virtual_column_layout::redraw(gcontext& context,const box& clip)
{
	// Remove margin.
	box ibox(_bbox-_margin);

	// Look for the first row with a lower edge which overlaps (or is
	// below) the clip box: _ymax[y0+1] + _ygap < clip.ymax().
	vector<int>::iterator yf0=upper_bound(
		_ymax.begin(),_ymax.end(),clip.ymax()-_ygap,greater<int>());
	size_type y0=yf0-_ymax.begin();
	if (y0) --y0;

	// Look for the first row with an upper edge which is below the
	// clip box: _ymax[y1] <= clip.ymin().
	vector<int>::iterator yf1=lower_bound(
		_ymax.begin(),_ymax.end(),clip.ymin(),greater<int>());
	size_type y1=yf1-_ymax.begin();
	if (y1>_ycells) y1=_ycells;

	// Redraw children.
	// For safety, use an inequality in the for loop.
	for (size_type y=y0;y<y1;++y)
	{
		// Create y-baseline set for just this cell.
		ybaseline_set ybs;
		ybs.add(cell_min_bbox(y),cell_ybaseline(y));

		// Construct bounding box for cell with respect to origin
		// of layout.
		box cbbox(ibox.xmin(),_ymax[y+1]+_ygap,ibox.xmax(),_ymax[y]);

		// Calculate offset from top left-hand corner of cell to
		// origin of child.
		int xoffset=_xbs.offset(xbaseline_left,cell_xbaseline(y),
			cbbox.xsize());
		int yoffset=ybs.offset(ybaseline_bottom,cell_ybaseline(y),
			cbbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(cbbox.xminymin()+coffset);

		// Redraw cell.
		context+=cpos;
		cell_redraw(y,context,clip+cpos);
		context-=cpos;
	}
	inherited::redraw(context,clip);
}

virtual_column_layout& virtual_column_layout::cells(size_type ycells)
{
	_ycells=ycells;
	_ymax.resize(ycells+1,0);
	invalidate();
	return *this;
}

virtual_column_layout& virtual_column_layout::ygap(int ygap)
{
	_ygap=ygap;
	invalidate();
	return *this;
}

virtual_column_layout& virtual_column_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

virtual_column_layout& virtual_column_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

virtual_column_layout::xbaseline_type virtual_column_layout::cell_xbaseline(
	size_type y) const
{
	return xbaseline_text;
}

virtual_column_layout::ybaseline_type virtual_column_layout::cell_ybaseline(
	size_type y) const
{
	return ybaseline_text;
}

void virtual_column_layout::cell_reformat(size_type y,const point& origin,
	const box &bbox)
{}

virtual_column_layout::size_type virtual_column_layout::find_y(const point& p)
{
	// Look for the first cell with a top edge which is below the point:
	// _ymax[x] <= p.x().
	vector<int>::const_iterator yf=
		lower_bound(_ymax.begin(),_ymax.end(),p.y(),greater<int>());
	if (yf==_ymax.begin()) return npos;
	size_type y=(yf-_ymax.begin())-1;

	// Return npos if point outside cell limits, otherwise return y.
	if ((p.y()>=_ymax[y])||(p.y()<_ymax[y+1]+_ygap)) return npos;
	return y;
}

}; /* namespace desktop */
}; /* namespace rtk */
