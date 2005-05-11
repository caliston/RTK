// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/virtual_grid_layout.h"

namespace rtk {
namespace desktop {

using rtk::util::divider;

virtual_grid_layout::virtual_grid_layout(size_type xcells,size_type ycells):
	_xcells(xcells),
	_ycells(ycells),
	_xmin(xcells+1,0),
	_ymax(ycells+1,0),
	_xbs(xcells),
	_ybs(ycells),
	_xgap(0),
	_ygap(0)
{}

virtual_grid_layout::~virtual_grid_layout()
{
	remove();
}

box virtual_grid_layout::auto_bbox() const
{
	// Reset baseline sets.
	for (size_type x=0;x!=_xcells;++x) _xbs[x]=xbaseline_set();
	for (size_type y=0;y!=_ycells;++y) _ybs[y]=ybaseline_set();

	// Request min_bbox for each cell.  Incorporate into appropriate
	// x-baseline and y-baseline set.
	for (size_type y=0;y!=_ycells;++y)
	{
		for (size_type x=0;x!=_xcells;++x)
		{
			box mcbbox=cell_min_bbox(x,y);
			_xbs[x].add(mcbbox,cell_xbaseline(x,y));
			_ybs[y].add(mcbbox,cell_ybaseline(x,y));
		}
	}

	// Calculate total width and height.
	int xsize=0;
	int ysize=0;
	for (size_type x=0;x!=_xcells;++x)
		xsize+=_xbs[x].xsize();
	for (size_type y=0;y!=_ycells;++y)
		ysize+=_ybs[y].ysize();

	// Add gaps to width and height.
	if (_xcells) xsize+=(_xcells-1)*_xgap;
	if (_ycells) ysize+=(_ycells-1)*_ygap;

	// Add margin to width and height
	xsize+=_margin.xsize();
	ysize+=_margin.ysize();

	// Construct minimum bounding box, with respect to top left-hand
	// corner of layout.
	box abbox(0,-ysize,xsize,0);

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

box virtual_grid_layout::bbox() const
{
	return _bbox;
}

void virtual_grid_layout::reformat(const point& origin,const box& pbbox)
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

	// Calculate excess in x-direction.
	int xexcess=_bbox.xsize()-abbox.xsize();
	if (xexcess<0) xexcess=0;
	int xspread=abbox.xsize();
	xspread-=_margin.xsize();
	if (_xcells) xspread-=(_xcells-1)*_xgap;
	if (xspread<0) xspread=0;
	divider xdiv(xexcess,xspread);

	// Set minimum x-coordinate for each column.
	int xpos=ibox.xmin();
	_xmin[0]=xpos;
	for (size_type x=0;x!=_xcells;++x)
	{
		int xsize=_xbs[x].xsize();
		xpos+=xsize+xdiv(xsize);
		xpos+=_xgap;
		_xmin[x+1]=xpos;
	}

	// Calculate excess in y-direction.
	int yexcess=_bbox.ysize()-abbox.ysize();
	if (yexcess<0) yexcess=0;
	int yspread=abbox.ysize();
	yspread-=_margin.ysize();
	if (_ycells) yspread-=(_ycells-1)*_ygap;
	if (yspread<0) yspread=0;
	divider ydiv(yexcess,yspread);

	// Set minimum and maximum y-coordinates for each row.
	int ypos=ibox.ymax();
	_ymax[0]=ypos;
	for (size_type y=0;y!=_ycells;++y)
	{
		int ysize=_ybs[y].ysize();
		ypos-=ysize+ydiv(ysize);
		ypos-=_ygap;
		_ymax[y+1]=ypos;
	}

	// Place children.
	for (size_type y=0;y!=_ycells;++y)
	{
		for (size_type x=0;x!=_xcells;++x)
		{
			// Construct bounding box for cell with respect to
			// origin of layout.
			box cbbox(_xmin[x],_ymax[y+1]+_ygap,_xmin[x+1]-_xgap,_ymax[y]);

			// Calculate offset from top left-hand corner of cell
			// to origin of child.
			int xoffset=_xbs[x].offset(xbaseline_left,cell_xbaseline(x,y),
				cbbox.xsize());
			int yoffset=_ybs[y].offset(ybaseline_bottom,cell_ybaseline(x,y),
				cbbox.ysize());
			point coffset(xoffset,yoffset);

			// Calculate origin of cell with respect to origin of
			// layout.
			point cpos(cbbox.xminymin()+coffset);

			// Reformat child.
			cell_reformat(x,y,cpos,cbbox-cpos);
		}
	}
}

void virtual_grid_layout::redraw(gcontext& context,const box& clip)
{
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

	// Look for the first row with a lower edge which overlaps (or is
	// below) the clip box: _ymax[y0+1] + _ygap < clip.ymax().
	std::vector<int>::iterator yf0=upper_bound(
		_ymax.begin(),_ymax.end(),clip.ymax()-_ygap,std::greater<int>());
	size_type y0=yf0-_ymax.begin();
	if (y0) --y0;

	// Look for the first row with an upper edge which is below the
	// clip box: _ymax[y1] <= clip.ymin().
	std::vector<int>::iterator yf1=lower_bound(
		_ymax.begin(),_ymax.end(),clip.ymin(),std::greater<int>());
	size_type y1=yf1-_ymax.begin();
	if (y1>_ycells) y1=_ycells;

	// Redraw children.
	// For safety, use inequalities in the for-loops.
	for (size_type y=y0;y<y1;++y)
	{
		for (size_type x=x0;x<x1;++x)
		{
			// Construct bounding box for cell with respect to
			// origin of layout.
			box cbbox(_xmin[x],_ymax[y+1]+_ygap,_xmin[x+1]-_xgap,_ymax[y]);

			// Calculate offset from top left-hand corner of cell
			// to origin of child.
			int xoffset=_xbs[x].offset(xbaseline_left,cell_xbaseline(x,y),
				cbbox.xsize());
			int yoffset=_ybs[y].offset(ybaseline_bottom,cell_ybaseline(x,y),
				cbbox.ysize());
			point coffset(xoffset,yoffset);

			// Calculate origin of cell with respect to origin of
			// layout.
			point cpos(cbbox.xminymin()+coffset);

			// Redraw cell.
			context+=cpos;
			cell_redraw(x,y,context,clip-cpos);
			context-=cpos;
		}
	}
	inherited::redraw(context,clip);
}

virtual_grid_layout& virtual_grid_layout::cells(size_type xcells,
	size_type ycells)
{
	_xcells=xcells;
	_ycells=ycells;
	_xmin.resize(xcells+1,0);
	_ymax.resize(ycells+1,0);
	_xbs.resize(xcells);
	_ybs.resize(ycells);
	invalidate();
	return *this;
}

virtual_grid_layout& virtual_grid_layout::xgap(int xgap)
{
	_xgap=xgap;
	invalidate();
	return *this;
}

virtual_grid_layout& virtual_grid_layout::ygap(int ygap)
{
	_ygap=ygap;
	invalidate();
	return *this;
}

virtual_grid_layout& virtual_grid_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

virtual_grid_layout& virtual_grid_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

virtual_grid_layout::xbaseline_type virtual_grid_layout::cell_xbaseline(
	size_type x,size_type y) const
{
	return xbaseline_text;
}

virtual_grid_layout::ybaseline_type virtual_grid_layout::cell_ybaseline(
	size_type x,size_type y) const
{
	return ybaseline_text;
}

void virtual_grid_layout::cell_reformat(size_type x,size_type y,
	const point& origin,const box &bbox)
{}

virtual_grid_layout::size_type virtual_grid_layout::find_x(const point& p)
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

virtual_grid_layout::size_type virtual_grid_layout::find_y(const point& p)
{
	// Look for the first cell with a top edge which is below the point:
	// _ymax[x] <= p.x().
	std::vector<int>::const_iterator yf=
		lower_bound(_ymax.begin(),_ymax.end(),p.y(),std::greater<int>());
	if (yf==_ymax.begin()) return npos;
	size_type y=(yf-_ymax.begin())-1;

	// Return npos if point outside cell limits, otherwise return y.
	if ((p.y()>=_ymax[y])||(p.y()<_ymax[y+1]+_ygap)) return npos;
	return y;
}

}; /* namespace desktop */
}; /* namespace rtk */
