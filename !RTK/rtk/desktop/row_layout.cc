// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/row_layout.h"

namespace rtk {
namespace desktop {

using rtk::util::divider;

row_layout::row_layout(size_type xcells):
	_components(xcells,0),
	_xmin(xcells+1,0),
	_xgap(0)
{}

row_layout::~row_layout()
{
	for (vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->remove();
	}
}

box row_layout::auto_bbox() const
{
	// Determine number of cells.
	size_type xcells=_components.size();

	// Reset y-baseline set.
	_ybs=ybaseline_set();

	// Request min_bbox for each cell.  Incorporate into y-baseline set
	// and total width.
	int xsize=0;
	vector<component*>::const_iterator i=_components.begin();
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

component* row_layout::find(const point& p) const
{
	vector<int>::const_iterator xf=
		lower_bound(_xmin.begin(),_xmin.end(),p.x()+1,less<int>());
	if (xf==_xmin.begin()) return 0;
	size_type x=(xf-_xmin.begin())-1;
	component* c=_components[x];
	if (!c) return 0;
	box cbbox=c->bbox()+c->origin();
	if (!(p<=cbbox)) return 0;
	return c;
}

box row_layout::bbox() const
{
	return _bbox;
}

void row_layout::resize() const
{
	for (vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->resize();
	}
	inherited::resize();
}

void row_layout::reformat(const point& origin,const box &bbox)
{
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

void row_layout::unformat()
{
	for (vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->unformat();
	}
}

void row_layout::redraw(gcontext& context,const box& clip)
{
	// Look for the first column with a right edge which overlaps (or is to
	// the right of) the clip box: _xmin[x0+1] -_xgap > clip.xmin().
	vector<int>::iterator xf0=upper_bound(
		_xmin.begin(),_xmin.end(),clip.xmin()+_xgap,less<int>());
	size_type x0=xf0-_xmin.begin();
	if (x0) --x0;

	// Look for the first column with a left edge which is to the right of
	// the clip box: _xmin[x1] >= clip.xmax().
	vector<int>::iterator xf1=lower_bound(
		_xmin.begin(),_xmin.end(),clip.xmax(),less<int>());
	size_type x1=xf1-_xmin.begin();
	if (x1>_components.size()) x1=_components.size();

	// Redraw children.
	// For safety, use an inequality in the for-loop.
	for (size_type x=x0;x<x1;++x)
	{
		if (component* c=_components[x])
		{
			point cpos=c->origin();
			context+=cpos;
			c->redraw(context,clip+cpos);
			context-=cpos;
		}
	}
	inherited::redraw(context,clip);
}

void row_layout::remove_notify(component& c)
{
	vector<component*>::iterator f=
		std::find(_components.begin(),_components.end(),&c);
	if (f!=_components.end())
	{
		*f=0;
		invalidate();
	}
}

row_layout& row_layout::cells(size_type xcells)
{
	for (size_type i=min(xcells,_components.size());i!=_components.size();++i)
		if (component* c=_components[i]) c->remove();
	_components.resize(xcells,0);
	_xmin.resize(xcells+1,0);
	invalidate();
	return *this;
}

row_layout& row_layout::add(component& c,size_type x)
{
	if (x==npos) x=_components.size();
	if (x>=_components.size()) cells(x+1);
	if (_components[x]) _components[x]->remove();
	_components[x]=&c;
	link_child(c);
	invalidate();
	return *this;
}

row_layout& row_layout::xgap(int xgap)
{
	_xgap=xgap;
	invalidate();
	return *this;
}

row_layout& row_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

row_layout& row_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

}; /* namespace desktop */
}; /* namespace rtk */
